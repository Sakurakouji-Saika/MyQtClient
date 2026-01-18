#ifndef APPEVENTBUS_H
#define APPEVENTBUS_H

#include <QObject>
#include <QJsonObject>

// 广播事件（好友上线，下线，发送消息，更新头像） 就写这里吧
class AppEventBus : public QObject
{
    Q_OBJECT
public:
    explicit AppEventBus(QObject *parent = nullptr);

    // 好友上下线状态管理
    void friendStatusChanged(const QJsonObject& json);

    // 消息到达
    void messageReceived(const QJsonObject& json);

    // 登录状态变化(不写了吧)
    void connectionStateChanged(const QJsonObject& json);

    // 好友更新头像
    void UpdateAvatar(const QJsonObject& json);

signals:
    void Friend_OnlineSignal(qint64 friend_uid,int state);

};

#endif // APPEVENTBUS_H
