#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include "../utils/appconfig.h"
#include <QQueue>

class ClientSocket : public QObject
{
    Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = nullptr);
    ~ClientSocket();

    // 设置当前用户ID
    QString GetUserId() const;
    void SetUserId(const QString &id);

    // 查看是否连接
    void CheckConnected();
    // 关闭链接
    void CloseConnected();

    // 连接到服务器
    void ConnectToHost(const QString &host,const int &port);
    void ConnectToHost(const QHostAddress &host , const int &port);

    //异步连接方法
    void connectToHostAsync();

    //消息发送方法
    void sendJsonMessage(const quint8 &type, const QJsonValue &dataVal);


signals:

    void signalMessage(const quint8&type,const QJsonValue &dataVal);
    void signalStatus(const quint8 &state);



public slots:
    // socket消息发送封装
    void SltSendMessage(const quint8 &type, const QJsonValue &dataVal);
    // 发送上线通知
    void SltSendOnline();
    // 发送下线通知
    void SltSendOffline();

private slots:

    // 与服务器断开连接
    void SltDisconnected();
    // 链接服务器
    void SltConnected();
    // tcp协议，消息处理
    void SltReadyRead();



private:

    QTcpSocket *m_tcpSocket;
    QString m_nId = AppConfig::instance().getUserID();

    QQueue<QPair<quint8, QJsonValue>> m_msgQueue; // 消息缓存队列
};

#endif // CLIENTSOCKET_H
