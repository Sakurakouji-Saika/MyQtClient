#include "FriendsResponse.h"


#include <QJsonObject>
#include <QJsonDocument>

// 解析 LastMessageInfo 的实现
std::optional<LastMessageInfo> _parseLastMessage(const QJsonValue &val)
{
    if (val.isNull() || !val.isObject())
        return std::nullopt;

    QJsonObject obj = val.toObject();
    LastMessageInfo lm;
    lm.id = obj.value("id").toInt(0);
    lm.type = obj.value("type").toString();
    lm.content = obj.value("content").toString();
    lm.senderId = obj.value("senderId").toInt(0);

    QString sentAtStr = obj.value("sentAt").toString();
    if (!sentAtStr.isEmpty()) {
        // 尝试按 ISO 格式解析（例如 "2025-12-04T11:02:06"）
        lm.sentAt = QDateTime::fromString(sentAtStr, Qt::ISODate);
        // 视需要可添加备用解析（带毫秒等）
        // if (!lm.sentAt.isValid()) lm.sentAt = QDateTime::fromString(sentAtStr, Qt::ISODateWithMs);
    }

    return lm.isValid() ? std::optional<LastMessageInfo>(lm) : std::nullopt;
}

// 解析整个响应的实现
FriendsResponse parseFriendsResponse(const QJsonObject &_resp)
{
    FriendsResponse resp;
    QJsonDocument doc(_resp);
    QJsonObject root = doc.object();

    resp.userId = root.value("user").toInt(0);

    // 先把 QJsonArray 存为局部变量，避免把临时放进 range-for
    QJsonArray fa = root.value("friends").toArray();

    // 使用索引循环并调用 at(i)
    for (int i = 0, n = fa.size(); i < n; ++i) {
        QJsonValue fv = fa.at(i); // 使用 at() 更明确且不会引起额外 detach
        if (!fv.isObject())
            continue;

        QJsonObject fo = fv.toObject();
        FriendInfo_sever fi;
        fi.id = fo.value("id").toInt(0);
        fi.username = fo.value("username").toString();
        fi.nickname = fo.value("nickname").toString();
        fi.avatarPath = fo.value("avatarPath").toString();
        fi.unreadCount = fo.value("unreadCount").toInt(0);

        // 解析 LastMessageInfo（可能为 null）
        fi.lastMessage = _parseLastMessage(fo.value("LastMessageInfo"));

        resp.friends.append(fi);
    }

    return resp;
}
