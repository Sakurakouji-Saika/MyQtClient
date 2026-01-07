#include "friendservice.h"

#include <QJsonObject>

#include "../PacketProcessor/packetprocessor.h"
#include "../../src/utils/comapi/Protocol.h"

FriendService::FriendService(PacketProcessor *processor, QObject *parent)
    : QObject{parent}
{
    m_pp = processor;
}

void FriendService::search_friends(qint64 uid)
{
    if(!m_pp){
        emit SearchFriednErrorSignals(QStringLiteral("FriendService::search_friends:: 包处理器不存在"));
        return;
    }

    QJsonObject request;
    request["type"] = static_cast<int>(Protocol::MessageType::SearchFriend);
    request["uid"] = uid;

    m_pp->sendRequest(request,[this](const QJsonObject &resp){


        bool ok = resp.value("ok").toBool();


        if(ok){
            qint64 uid = resp.value("uid").toString().toLongLong();
            QString username = resp.value("username").toString();
            QString nickname = resp.value("nickname").toString();
            qint64 avatar_file_id = resp.value("file_avatar_id").toString().toLongLong();
            QString avatar = resp.value("avatar").toString();
            emit SearchFriednSuccessSignals(uid,username,nickname,avatar_file_id,avatar);
            return;
        }else{
            emit SearchFriednErrorSignals(resp.value("err").toString());
        }
    },-1);

}

void FriendService::add_friend(qint64 requester_uid, qint64 target_uid)
{
    if(!m_pp){
        emit AddFriendErrorSignals(QStringLiteral("FriendService::add_friend:: 包处理器不存在"));
        return;
    }

    QJsonObject request;
    request["type"] = static_cast<int>(Protocol::MessageType::AddFriend);

    // requester_uid：表示申请好友的用户ID（即发起请求的用户）。
    // target_uid：表示被添加的用户ID（即好友请求的目标用户）。


    request["requester_uid"] = requester_uid;
    request["target_uid"] = target_uid;

    m_pp->sendRequest(request,[this](const QJsonObject &resp){

        bool ok = resp.value("ok").toBool();
        if(ok){
            emit AddFriendSuccessSignals();
            return;
        }else{
            emit AddFriendErrorSignals(resp.value("error").toString());
        }
    },-1);

}
