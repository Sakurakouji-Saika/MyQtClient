#ifndef FRIENDINFO_H
#define FRIENDINFO_H
#include <QString>
#include <QDateTime>

// ✅ FriendInfo.h
struct FriendInfo
{
    int id;
    int friend_id;
    QString display_name;
    QString avatar;
    int status;
    QString remark;
    qint64 created_at;

    FriendInfo()
        : id(-1), status(0), created_at(QDateTime::currentSecsSinceEpoch())
    {}

    FriendInfo(const int &t_id,
                        const int& fid,
                        const QString& name,
                        const QString& avatarPath,
                        int status,
                        const QString& remark,
                        qint64 createdAt)
        : id(t_id),
        friend_id(fid),
        display_name(name),
        avatar(avatarPath),
        status(status),
        remark(remark),
        created_at(createdAt)
    {}
};



#endif // FRIENDINFO_H
