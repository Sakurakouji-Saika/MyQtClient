#include "packetprocessor.h"
#include "../socketclient.h"
#include <QJsonParseError>
#include <QJsonDocument>
#include <QTimer>
#include <QDebug>

PacketProcessor::PacketProcessor(SocketClient *_socketClient, QObject *parent)
    : QObject(parent), m_net(_socketClient)
{
    if (m_net) {
        connect(m_net, &SocketClient::frameReceived, this, &PacketProcessor::onFrame);
    }
}

PacketProcessor::~PacketProcessor()
{
    if (m_net) {
        disconnect(m_net, &SocketClient::frameReceived, this, &PacketProcessor::onFrame);
    }

    // 清理 pending timers & 回调（以断线错误通知）
    clearPending();

    QMutexLocker locker(&m_mutex);
    m_outgoingFilters.clear();
    m_handlers.clear();
}

void PacketProcessor::setDefaultTimeoutMs(int ms)
{
    QMutexLocker locker(&m_mutex);
    m_defaultTimeoutMs = ms;
}

int PacketProcessor::defaultTimeoutMs() const
{
    QMutexLocker locker(&m_mutex);
    return m_defaultTimeoutMs;
}

void PacketProcessor::registerHandler(int type, JsonHandler handler)
{
    QMutexLocker locker(&m_mutex);
    m_handlers[type] = std::move(handler);
}

void PacketProcessor::unregisterHandler(int type)
{
    QMutexLocker locker(&m_mutex);
    m_handlers.remove(type);
}

qint64 PacketProcessor::addOutgoingFilter(OutgoingFilter f)
{
    QMutexLocker locker(&m_mutex);
    m_nextFilterId++;
    m_outgoingFilters[m_nextFilterId] = std::move(f);
    return m_nextFilterId;
}

void PacketProcessor::removeOutgoingFilter(qint64 filterId)
{
    QMutexLocker locker(&m_mutex);
    m_outgoingFilters.remove(filterId);
}

void PacketProcessor::sendJson(const QJsonObject &obj)
{
    if (!m_net) return;

    QJsonObject copy = obj;

    // snapshot filters
    QMap<qint64, OutgoingFilter> filtersCopy;
    {
        QMutexLocker locker(&m_mutex);
        filtersCopy = m_outgoingFilters;
    }

    for (auto it = filtersCopy.begin(); it != filtersCopy.end(); ++it) {
        try {
            it.value()(copy);
        } catch (...) {
            qDebug() << "PacketProcessor: outgoing filter threw exception";
        }
    }

    m_net->sendJson(copy);
}

qint64 PacketProcessor::sendRequest(const QJsonObject &obj, ResponseCallback cb, int timeoutMs)
{
    if (!m_net) return -1;

    // determine effective timeout
    int effectiveTimeout;
    {
        QMutexLocker locker(&m_mutex);
        if (timeoutMs < 0) effectiveTimeout = m_defaultTimeoutMs;
        else effectiveTimeout = timeoutMs; // can be 0 meaning disable timeout
    }

    qint64 seq;
    {
        QMutexLocker locker(&m_mutex);
        m_nextSeq++;
        seq = m_nextSeq;
        Pending p;
        p.cb = std::move(cb);
        p.timeoutTimer = nullptr;
        m_pending[seq] = std::move(p);
    }

    // assemble JSON and apply filters
    QJsonObject copy = obj;
    copy["seq"] = (double)seq;

    QMap<qint64, OutgoingFilter> filtersCopy;
    {
        QMutexLocker locker(&m_mutex);
        filtersCopy = m_outgoingFilters;
    }

    for (auto it = filtersCopy.begin(); it != filtersCopy.end(); ++it) {
        try { it.value()(copy); } catch(...) { qDebug() << "PacketProcessor: outgoing filter threw"; }
    }

    // set up timeout timer if requested (effectiveTimeout > 0)
    if (effectiveTimeout > 0) {
        QTimer *t = new QTimer(this);
        t->setSingleShot(true);
        t->setInterval(effectiveTimeout);
        connect(t, &QTimer::timeout, this, [this, seq]() {
            ResponseCallback cb;
            {
                QMutexLocker locker(&m_mutex);
                if (!m_pending.contains(seq)) return; // already handled/cancelled
                cb = std::move(m_pending[seq].cb);
                // delete timer pointer reference (timer is 'this' parent so deleteLater below is fine)
                if (m_pending[seq].timeoutTimer) {
                    m_pending[seq].timeoutTimer->stop();
                    m_pending[seq].timeoutTimer->deleteLater();
                }
                m_pending.remove(seq);
            }
            if (cb) {
                QJsonObject err = PacketProcessor::makeErrorObject(QStringLiteral("timeout"), -1);
                try { cb(err); } catch(...) { qDebug() << "PacketProcessor: pending callback threw on timeout"; }
            }
        });

        // attach timer pointer to pending
        {
            QMutexLocker locker(&m_mutex);
            if (m_pending.contains(seq)) {
                m_pending[seq].timeoutTimer = t;
            } else {
                // race: pending gone, cleanup timer
                t->deleteLater();
            }
        }
        t->start();
    }

    // finally send
    m_net->sendJson(copy);
    return seq;
}

void PacketProcessor::cancelRequest(qint64 seq)
{
    ResponseCallback cb;
    {
        QMutexLocker locker(&m_mutex);
        if (!m_pending.contains(seq)) return;
        cb = std::move(m_pending[seq].cb);
        if (m_pending[seq].timeoutTimer) {
            m_pending[seq].timeoutTimer->stop();
            m_pending[seq].timeoutTimer->deleteLater();
        }
        m_pending.remove(seq);
    }
    // Optionally notify caller via cb that it was cancelled:
    if (cb) {
        QJsonObject err = makeErrorObject(QStringLiteral("cancelled"), -2);
        try { cb(err); } catch(...) { qDebug() << "PacketProcessor: pending callback threw on cancel"; }
    }
}

int PacketProcessor::pendingCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_pending.size();
}

void PacketProcessor::clearPending()
{
    QMap<qint64, Pending> copy;
    {
        QMutexLocker locker(&m_mutex);
        copy = m_pending;
        m_pending.clear();
    }

    // notify all pending with an error (e.g. disconnected)
    for (auto it = copy.begin(); it != copy.end(); ++it) {
        if (it->timeoutTimer) {
            it->timeoutTimer->stop();
            it->timeoutTimer->deleteLater();
        }
        if (it->cb) {
            QJsonObject err = makeErrorObject(QStringLiteral("cleared"), -3);
            try { it->cb(err); } catch(...) { qDebug() << "PacketProcessor: pending callback threw on clear"; }
        }
    }
}

void PacketProcessor::onFrame(const QByteArray &payload)
{
    // try parse json
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(payload, &err);

    if (err.error == QJsonParseError::NoError && doc.isObject()) {
        QJsonObject obj = doc.object();


        // seq matching
        qint64 seq = (qint64)obj.value("seq").toDouble(0);

        if (seq > 0) {
            ResponseCallback cb;
            {
                QMutexLocker locker(&m_mutex);
                if (m_pending.contains(seq)) {
                    cb = std::move(m_pending[seq].cb);
                    if (m_pending[seq].timeoutTimer) {
                        m_pending[seq].timeoutTimer->stop();
                        m_pending[seq].timeoutTimer->deleteLater();
                    }
                    m_pending.remove(seq);
                }
            }

            if (cb) {
                try { cb(obj); } catch(...) {
                    qDebug() << "PacketProcessor: pending callback threw";
                }

                return; // handled as response
            }
            // else continue to type dispatch (maybe it's a pushed event carrying seq)
        }

        // type dispatch
        if (!obj.contains("type")) {
            qDebug() << "PacketProcessor::onFrame: missing type field";
            emit rawFrameReceived(payload);
            return;
        }

        int type = obj.value("type").toInt();
        JsonHandler handler;
        {
            QMutexLocker locker(&m_mutex);
            if (m_handlers.contains(type)) handler = m_handlers[type];
        }

        if (handler) {
            try { handler(obj); } catch(...) { qDebug() << "PacketProcessor: handler threw"; }
        } else {
            emit unknownTypeReceived(obj);
        }
    } else {
        // binary or non-json
        emit rawFrameReceived(payload);
    }
}

QJsonObject PacketProcessor::makeErrorObject(const QString &reason, int code)
{
    QJsonObject obj;
    obj["ok"] = false;
    obj["error"] = reason;
    obj["code"] = code;
    return obj;
}
