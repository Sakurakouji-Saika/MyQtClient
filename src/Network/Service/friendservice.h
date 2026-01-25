#ifndef FRIENDSERVICE_H
#define FRIENDSERVICE_H

#include <QObject>
#include <QPointer>
#include <QJsonArray>

class PacketProcessor;

#include "../models/userinfo.h"

class FriendService : public QObject
{
    Q_OBJECT
public:
    explicit FriendService(PacketProcessor *processor, QObject *parent = nullptr);

    void search_friends(qint64 uid);
    void add_friend(qint64 requester_uid, qint64 target_uid);
    void get_Friend_request(qint64 uid);
    void send_agree_friend(qint64 _agree_uid);

    void Delete_friend(qint64 friend_uid);

    // 发送消息(重要)
    void SendMessage(qint64 sender_id, qint64 receiver_id,int msgType, QString msgContent, qint64 file_id, qint64 send_at);

    void SendReceivedMsgACK(qint64 msgID);

    void SendMessageReadReceipt(qint64 friendID, qint64 userID, qint64 msgID = -1);


signals:
    void SearchFriednErrorSignals(QString error);
    void SearchFriednSuccessSignals(qint64 uid, QString userName, QString nickname, qint64 avatar_file_id, QString avatar);


    void AddFriendErrorSignals(QString error);
    void AddFriendSuccessSignals();

    void GetFriendRequestListSuccessSignals(QList<UserInfo> datalists);
    void GetFriendRequestListErrorSignals();

    // void DowloadFriendAvatarSignals(qint64 uid,qint64 file_id);
    void InsetFriendData(qint64 uid,QString avatar,QString nickname,QString username,qint64 file_avatar_id);

    void DeleteFriendSuccessSignals(qint64 uid);
    void DeleteFriendErrorSignals(QString error);


private:

    QPointer<PacketProcessor> m_pp;
};

#endif // FRIENDSERVICE_H
