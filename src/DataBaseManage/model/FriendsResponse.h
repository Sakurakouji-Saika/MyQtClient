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

// 最后一条消息结构体
struct LastMessageInfo {
    int id = 0;         // 消息 id（或接收者 id，按服务端定义）
    QString type;       // type: "text","image","file" 等
    QString content;    // 具体内容, 文本直接写入，图片/文件写地址
    int senderId = 0;   // 发送者ID
    QDateTime sentAt;   // 发送时间

    bool isValid() const { return id > 0; }
};

// 好友信息结构体
struct FriendInfo_sever {
    int id = 0;
    QString username;
    QString nickname;
    QString avatarPath;
    QDateTime lastSeen;
    QDateTime friendedAt;
    qint64 avatar_file_id;
    int unreadCount = 0;

    std::optional<LastMessageInfo> lastMessage;

    QString displayName() const {
        return nickname.isEmpty() ? username : nickname;
    }

    bool hasUnread() const { return unreadCount > 0; }
    bool isValid() const { return id > 0; }
};

struct FriendsResponse {
    int userId = 0;
    QList<FriendInfo_sever> friends;

    bool isValid() const { return userId > 0; }
};

// 解析 LastMessageInfo 的实现
std::optional<LastMessageInfo> _parseLastMessage(const QJsonValue &val);

// 解析整个响应的实现
FriendsResponse parseFriendsResponse(const QJsonObject &_resp);


static int messageTypeToInt(const QString &type) {
    if (type == "text") return 0;
    if (type == "image") return 1;
    if (type == "file") return 2;
    return 0; // 默认文本
}

#endif // GETFRIENDS_H
