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
