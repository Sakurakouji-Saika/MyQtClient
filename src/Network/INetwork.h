#ifndef INETWORK_H
#define INETWORK_H

#include <QObject>
#include <QJsonValue>
#include <QJsonObject>

class INetwork : public QObject
{
    Q_OBJECT
public:
    explicit INetwork(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~INetwork() override = default;

    // 发送消息（业务层使用）
    virtual void sendMessage(quint8 type, const QJsonValue &dataVal) = 0;

    // 检查/连接/关闭
    virtual void checkConnected() = 0;
    virtual void connectToHostAsync() = 0;
    virtual void closeConnected() = 0;

signals:
    // 状态改变（例如 LoginSuccess / LoginFail / ConnectedHost 等）
    void statusChanged(const quint8 &state);
    // 收到消息（type, dataVal）
    void messageReceived(const quint8 &type, const QJsonValue &dataVal);

};
#endif // INETWORK_H
