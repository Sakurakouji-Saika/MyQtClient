#ifndef GETFRIENDS_H
#define GETFRIENDS_H

#include <QString>
#include <QDateTime>
#include <optional>
#include <QJsonValue>
#include <QJsonArray>
#include <QList>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>


// 好友信息结构体
struct FriendInfo_sever {
    int id = 0;
    QString username;
    QString nickname;
    QString avatarPath;
    QDateTime lastSeen;
    QDateTime friendedAt;
    qint64 avatar_file_id;
    bool state;


    QString displayName() const {
        return nickname.isEmpty() ? username : nickname;
    }

    bool isValid() const { return id > 0; }
};

struct FriendsResponse {
    int userId = 0;
    QList<FriendInfo_sever> friends;

    bool isValid() const { return userId > 0; }
};


// 解析整个响应的实现
FriendsResponse parseFriendsResponse(const QJsonObject &_resp);


static int messageTypeToInt(const QString &type) {
    if (type == "text") return 0;
    if (type == "image") return 1;
    if (type == "file") return 2;
    return 0; // 默认文本
}

#endif // GETFRIENDS_H
