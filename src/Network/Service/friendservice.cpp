#include "friendservice.h"

#include <QJsonObject>
#include <QMessageBox>

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

void FriendService::get_Friend_request(qint64 uid)
{
    if(!m_pp){
        emit SearchFriednErrorSignals(QStringLiteral("FriendService::get_Friend_request:: 包处理器不存在"));
        return;
    }

    QJsonObject request;
    request["type"] = static_cast<int>(Protocol::MessageType::FriendRequestList);
    request["receiver_id"] = uid;

    m_pp->sendRequest(request,[this](const QJsonObject &resp){

        bool ok = resp.value("ok").toBool();


        qDebug() << "FriendService::get_Friend_request(qint64 uid)::sendRequest" << resp;

        if(ok){

            int friendCount = resp.value("count").toInt();

            QJsonArray dataArray = resp.value("data").toArray();


            QList<UserInfo> listdata;

            for (const QJsonValue &val : dataArray) {
                if (!val.isObject())
                    continue;

                UserInfo temp;

                QJsonObject obj = val.toObject();

                temp.avatar     = obj.value("avatar").toString();
                temp.nickname   = obj.value("nickname").toString();
                temp.username   = obj.value("username").toString();
                temp.status     = obj.value("status").toInt();
                temp.userId     = obj.value("user_id").toInteger();
                temp.created_at = obj.value("created_at").toInteger();

                listdata.append(temp);
            }

            emit GetFriendRequestListSuccessSignals(listdata);

        }else{
            emit GetFriendRequestListErrorSignals();
        }
    },-1);

}

void FriendService::send_agree_friend(qint64 _agree_uid)
{
    if(!m_pp){
        emit AddFriendErrorSignals(QStringLiteral("FriendService::send_agree_friend:: 包处理器不存在"));
        return;
    }

    QJsonObject request;
    request["type"] = static_cast<int>(Protocol::MessageType::AgreeFriend);
    request["agree_uid"] = _agree_uid;

    m_pp->sendRequest(request,[this](const QJsonObject &resp){

        bool ok = resp.value("ok").toBool();

        if(ok){
            QMessageBox::critical(
                nullptr,
                QObject::tr("程序：添加好友"),
                QString("添加好友成功"),
                QMessageBox::Ok
                );

            qint64 uid = resp.value("uid").toInteger();
            QString avatar = resp.value("avatar").toString();
            QString nickname = resp.value("nickname").toString();
            QString username = resp.value("username").toString();
            qint64 file_avatar_id = resp.value("file_avatar_id").toInteger();

            emit InsetFriendData(uid,avatar,nickname,username,file_avatar_id);

        }else{

            QString error  = resp.value("error").toString();

            QMessageBox::critical(
                nullptr,
                QObject::tr("程序：添加好友：：提示"),
                QString("添加好友失败 || 出错:") + error,
                QMessageBox::Ok
                );
        }
    },-1);

}

void FriendService::Delete_friend(qint64 friend_uid)
{
    if(!m_pp){
        emit AddFriendErrorSignals(QStringLiteral("FriendService::Delete_friend:: 包处理器不存在"));
        return;
    }

    QJsonObject request;
    request["type"] = static_cast<int>(Protocol::MessageType::DeleteFriend);
    request["friend_uid"] = friend_uid;

    m_pp->sendRequest(request,[this](const QJsonObject &resp){

        qDebug() << "FriendService::Delete_friend::" << resp;
        bool ok = resp.value("ok").toBool();

        if(ok){
            qint64 uid = resp.value("friend_uid").toInteger();
            emit DeleteFriendSuccessSignals(uid);

        }else{


        }
    });
}
