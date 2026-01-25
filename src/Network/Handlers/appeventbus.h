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

    // 添加好友成功信号
    void friendAdded(const QJsonObject& json);

    // 删除好友成功信号
    void RemovedByFriend(const QJsonObject& json);

    // 收到好友发来的消息(const QJsonObject& json);
    void ReceiveMessage(const QJsonObject& json);

signals:
    void Friend_OnlineSignal(qint64 friend_uid,int state);

    void UpdateAvatarSignal(qint64 uid, qint64 file_id);

    void RemovedByFriendSignal(qint64 uid);

    void friendAddedSignal(qint64 uid, QString avatarName, qint64 avatar_file_id, QString nickname, int status, QString username);

    void ReceiveNewMessageSignals(qint64 file_id, QString msgContent, int msgType, qint64 send_at, qint64 sender_id, qint64 msgId, qint64 receiver_id);

    void ReceiveNewMsgSuccessSignals(qint64 msgID);
};

#endif // APPEVENTBUS_H
