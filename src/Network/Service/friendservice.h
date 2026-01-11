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


signals:
    void SearchFriednErrorSignals(QString error);
    void SearchFriednSuccessSignals(qint64 uid, QString userName, QString nickname, qint64 avatar_file_id, QString avatar);


    void AddFriendErrorSignals(QString error);
    void AddFriendSuccessSignals();

    void GetFriendRequestListSuccessSignals(QList<UserInfo> datalists);
    void GetFriendRequestListErrorSignals();

private:

    QPointer<PacketProcessor> m_pp;
};

#endif // FRIENDSERVICE_H
