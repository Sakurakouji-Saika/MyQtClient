#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QJsonObject>

class SocketClient : public QObject
{
    Q_OBJECT
public:
    explicit SocketClient(QObject *parent = nullptr);
    ~SocketClient() override;

    void connectToServer(const QString &host,quint16 port);

    void disconnectFromServer();

    void sendJson(const QJsonObject &obj);


    void sendRaw(const QByteArray &payload);
    // 发送原始 payload（注意：payload 不含长度前缀，类会封装）

    bool isConnected() const;   // socket 状态查询

    void closeConnection(bool graceful = true);

    void setUserId(qint64 uid) { m_userId = uid; }

    qint64 userId() const { return m_userId; }


signals:

    void connected();
    void disconnected();
    void frameReceived(const QByteArray &payload);
    void errorOccurred(const QString &err);

private slots:

    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:

    void processBuff();     // 缓冲区

private:

    QTcpSocket *m_socket = nullptr;
    QByteArray  m_buffer;                    // 积累的缓冲区数据
    quint32     m_expectedSize   = 0;        // 下一个 payload 期望大小
    QTimer*     m_heartbeatTimer = nullptr;  // 心跳定时器
    qint64      m_userId         = -1;       // 用户ID
};

#endif // SOCKETCLIENT_H
