#ifndef USERINFO_H
#define USERINFO_H

#include <QString>

struct UserInfo {
    qint64 userId;
    QString username;
    QString nickname;
    QString avatar;
    int status;
    qint64 created_at;
};

#endif // USERINFO_H
