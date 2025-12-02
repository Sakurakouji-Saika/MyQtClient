#ifndef PACKETPROCESSOR_H
#define PACKETPROCESSOR_H

#include <QObject>
#include <QByteArray>
#include <QJsonObject>
#include <QMap>
#include <QMutex>
#include <functional>
#include <QPointer>

class SocketClient;
class QTimer;

using JsonHandler = std::function<void(const QJsonObject&)>;
using ResponseCallback = std::function<void(const QJsonObject&)>; // 请求的回调
using OutgoingFilter = std::function<void(QJsonObject&)>;

class PacketProcessor : public QObject
{
    Q_OBJECT
public:
    explicit PacketProcessor(SocketClient* socketClient, QObject *parent = nullptr);
    ~PacketProcessor() override;

    // 注册/注销基于 type 的处理器
    void registerHandler(int type, JsonHandler handler);
    void unregisterHandler(int type);

    // 发送普通 JSON（notification / event），会执行 outgoing filters
    void sendJson(const QJsonObject &obj);

    // 发送带 seq 的请求（内置超时管理）
    // timeoutMs: >0 使用该超时； ==0 表示不启用内部超时（上层自己管理）
    // 返回分配的 seq (>0)，失败返回 -1
    qint64 sendRequest(const QJsonObject &obj, ResponseCallback cb, int timeoutMs = -1);

    // 取消单个 pending 请求（例如页面销毁、手动取消）
    void cancelRequest(qint64 seq);

    // 清除所有 pending（例如断线时调用），并会以 error 回调通知所有 pending
    void clearPending();

    // pending 数量
    int pendingCount() const;

    // outgoing filter 管理：add 返回 filterId，remove 按 filterId 注销
    qint64 addOutgoingFilter(OutgoingFilter f);
    void removeOutgoingFilter(qint64 filterId);

    // 默认超时设置（sendRequest 中 timeoutMs < 0 时使用该值）
    void setDefaultTimeoutMs(int ms);
    int defaultTimeoutMs() const;

signals:
    // 当解析不到 JSON（或业务想直接处理原始二进制数据）时会发出
    void rawFrameReceived(const QByteArray &payload);

    // 未知 type 时发出（可选监听）
    void unknownTypeReceived(const QJsonObject &obj);

private slots:
    void onFrame(const QByteArray &payload);

private:
    // helper: 生成统一的 error object 传给回调
    static QJsonObject makeErrorObject(const QString &reason, int code = -1);

private:
    QPointer<SocketClient> m_net;

    // type -> handler
    QMap<int, JsonHandler> m_handlers;

    // pending 回调管理（含 timer 指针）
    struct Pending {
        ResponseCallback cb;
        QTimer* timeoutTimer = nullptr; // owned by this (parent set to PacketProcessor)
    };
    qint64 m_nextSeq = 0;
    QMap<qint64, Pending> m_pending;

    // outgoing filters 管理（filterId -> function）
    qint64 m_nextFilterId = 0;
    QMap<qint64, OutgoingFilter> m_outgoingFilters;

    // 默认超时（毫秒），-1 表示默认不启用超时
    int m_defaultTimeoutMs = 3000;

    // 保护关键成员
    mutable QMutex m_mutex;
};

#endif // PACKETPROCESSOR_H
