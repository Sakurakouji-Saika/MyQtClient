#include "socketclient.h"

#include <QJsonDocument>
#include "../utils/utils.h"
#include "../utils/comapi/Protocol.h"

SocketClient::SocketClient(QObject *parent) :
    QObject(parent),
    m_socket(new QTcpSocket(this)),
    m_heartbeatTimer(new QTimer(this))
{
    connect(m_socket,&QTcpSocket::connected,this,&SocketClient::onConnected);
    connect(m_socket,&QTcpSocket::disconnected,this,&SocketClient::onDisconnected);
    connect(m_socket,&QTcpSocket::readyRead,this,&SocketClient::onReadyRead);
    connect(m_socket,&QTcpSocket::errorOccurred,this,&SocketClient::onSocketError);

    m_heartbeatTimer->setInterval(9 * 1000);    //9秒，服务端那边是10秒。
    connect(m_heartbeatTimer,&QTimer::timeout,this,[this](){
        if(isConnected()){
            QJsonObject heartbeat;
            int typeint = static_cast<int>(Protocol::MessageType::HeartbeatPing);
            heartbeat["type"] = typeint;
            // 这里还没写完...

            sendJson(heartbeat);
        }
    });

}

SocketClient::~SocketClient()
{
    if (m_socket) {
        if (m_socket->state() == QAbstractSocket::ConnectedState) {
            m_socket->disconnectFromHost();
        }
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}

void SocketClient::connectToServer(const QString &host, quint16 port)
{
    if(m_socket->state() == QAbstractSocket::ConnectedState){
        m_socket->disconnectFromHost();
    }

    m_buffer.clear();
    m_expectedSize = 0;
    m_socket->connectToHost(host,port);

}

void SocketClient::disconnectFromServer()
{
    if(m_socket->state() == QAbstractSocket::ConnectedState){
        m_socket->disconnectFromHost();
    }
}

void SocketClient::sendJson(const QJsonObject &obj)
{
    QJsonDocument doc(obj);



    QByteArray payload = doc.toJson(QJsonDocument::Compact);
    sendRaw(payload);
}

void SocketClient::sendRaw(const QByteArray &payload)
{
    if(!isConnected()){
        qDebug() << "SocketClient::sendRaw::发送消息失败:与服务器失去链接.";
        return;
    }

    QByteArray packet = uint32ToBE((quint32)payload.size()) + payload;
    qint64 written = m_socket->write(packet);
    if(written <= 0){
        qDebug() << "SocketClient::sendRaw::发送消息失败,失败原因:" << m_socket->errorString();
    }else{
        m_socket->flush();
    }
}

bool SocketClient::isConnected() const
{
    return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

void SocketClient::closeConnection(bool graceful)
{
    if(!m_socket){
        return;
    }

    if(m_heartbeatTimer->isActive()){
        m_heartbeatTimer->stop();
    }

    if(graceful){
        m_socket->disconnect();
        m_socket->disconnectFromHost();

        if(m_socket->state() !=QAbstractSocket::UnconnectedState){
            m_socket->abort();
        }
    }else{
        m_socket->disconnect(this);
        m_socket->abort();
    }

    m_buffer.clear();
    m_expectedSize = 0;


    m_socket->deleteLater();
    m_socket = nullptr;

}

void SocketClient::onConnected()
{
    qDebug() << "SocketClient::onConnected:客户端已连接上服务器.";

    m_heartbeatTimer->start(); // 启动心跳
    emit connected();
}

void SocketClient::onDisconnected()
{
    // 清理缓冲状态
    m_buffer.clear();
    m_expectedSize = 0;

    m_heartbeatTimer->stop(); // 停止心跳
    emit disconnected();

}

void SocketClient::onReadyRead()
{

    if (!m_socket) return;

    // 每次读取到字节流就往 缓冲区里面写入
    QByteArray chunk = m_socket->readAll();
    if (chunk.isEmpty()) return;

    m_buffer.append(chunk);

    processBuff();
}

void SocketClient::onSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() <<"scoket 错误,套接字描述符:[" + QString::number(socketError) + "]. " +
                    "错误内容为:" + m_socket->errorString();
}

void SocketClient::processBuff()
{
    // 安全上限
    constexpr quint32 MAX_FRAME_SIZE = 4UL * 1024 * 1024 * 1024 - 1; // 约 4 GB

    while(true){
        if(m_expectedSize == 0){
            if (m_buffer.size() < 4) break; // 不够长度字段
        }

        QByteArray lenBytes = m_buffer.left(4); // 读取网络帧 payload 长度 (4字节)


        m_expectedSize = readUint32BE(lenBytes);

        if(m_expectedSize == 0){
            m_buffer.remove(0, 4);
            continue;
        }

        if (m_expectedSize > MAX_FRAME_SIZE) {
            qDebug() << "帧大小超出上限: " + QString::number(m_expectedSize);
            return; // 停止当前解析
        }

        if (static_cast<quint64>(m_buffer.size()) < 4 + static_cast<quint64>(m_expectedSize)) {
            break;
        }


        m_buffer.remove(0,4);
        QByteArray payload = m_buffer.left(m_expectedSize);
        m_buffer.remove(0,m_expectedSize);
        m_expectedSize = 0;

        emit frameReceived(payload);
    }
}


