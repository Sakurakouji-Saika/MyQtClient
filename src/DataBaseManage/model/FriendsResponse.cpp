#include "FriendsResponse.h"


#include <QJsonObject>
#include <QJsonDocument>


// 解析整个响应的实现
FriendsResponse parseFriendsResponse(const QJsonObject &_resp)
{
    FriendsResponse resp;
    QJsonDocument doc(_resp);
    QJsonObject root = doc.object();

    resp.userId = root.value("user").toInt(0);

    QJsonArray fa = root.value("friends").toArray();

    for (int i = 0, n = fa.size(); i < n; ++i) {
        QJsonValue fv = fa.at(i);
        if (!fv.isObject())
            continue;

        QJsonObject fo = fv.toObject();
        FriendInfo_sever fi;
        fi.id = fo.value("id").toInt(0);
        fi.username = fo.value("username").toString();
        fi.nickname = fo.value("nickname").toString();
        fi.avatar_file_id = fo.value("avatar_file_id").toDouble();
        fi.avatarPath = fo.value("avatarPath").toString();
        fi.state = fo.value("state").toBool();

        resp.friends.append(fi);
    }

    return resp;
}
