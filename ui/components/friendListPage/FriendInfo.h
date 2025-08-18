// FriendInfo.h
#ifndef FRIENDINFO_H
#define FRIENDINFO_H

#include <QString>

struct FriendInfo {
    QString id;
    QString name;
    QString avatarUrl;
    bool    isOnline;
};

#endif // FRIENDINFO_H
