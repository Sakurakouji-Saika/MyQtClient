#include "authservice.h"
#include <QJsonObject>
#include <QString>
#include "../PacketProcessor/packetprocessor.h"
#include "../../utils/comapi/Protocol.h"

AuthService::AuthService(PacketProcessor *processor, QObject *parent)
    : QObject{parent},m_processor(processor)
{

}

AuthService::~AuthService()
{

}

void AuthService::login(const QString &username, const QString &password, int timeoutMs)
{
    if(!m_processor){
        emit loginFailed(QStringLiteral("AuthService::login::内部错误:没有PacketProcessor"));
        return;
    }

    QJsonObject req;
    req["type"] = static_cast<int>(Protocol::MessageType::Login);
    req["username"] = username;
    req["password"] = password;

    m_processor->sendRequest(req,[this](const QJsonObject &resp){
        bool ok = resp.value("ok").toBool(false);
        if(!ok){

            qDebug() << "登录消息:" << resp;
            QString reason = resp.value("error").toString("服务端错误信息提示为空");
            emit loginFailed(reason);
            return;
        }

        emit loginSucceeded(resp.value("user").toObject());

    },timeoutMs);

}

void AuthService::registration(const QString &username, const QString &password, const QString &email, int timeoutMs)
{
    if(!m_processor){
        emit loginFailed(QStringLiteral("AuthService::login::内部错误:没有PacketProcessor"));
        return;
    }

    QJsonObject req;
    req["type"] = static_cast<int>(Protocol::MessageType::Register);
    req["username"] = username;
    req["password"] = password;
    req["email"] = email;
    req["nickname"] = username;

    m_processor->sendRequest(req,[this,username](const QJsonObject &resp){

        bool ok = resp.value("ok").toBool(false);

        if(ok){
            QString s_uid = resp.value("id").toString();
            qint64 ll_uid = s_uid.toLongLong();
            emit registrationSucceede(ll_uid);
        }else{
            emit regostrationFailed(resp.value("error").toString());
        }

    },timeoutMs);

}

void AuthService::GetMyFriends(qint64 &id, int timeoutMs)
{
    if(!m_processor){
        emit loginFailed(QStringLiteral("AuthService::login::内部错误:没有PacketProcessor"));
        return;
    }

    QJsonObject req;
    req["type"] = static_cast<int>(Protocol::MessageType::GetMyFriends);
    req["user_id"] = id;


    m_processor->sendRequest(req,[this](const QJsonObject &resp){
        bool ok = resp.value("ok").toBool(false);



        if(!ok){
            QString reason = resp.value("error").toString("服务端错误信息提示为空");
            emit GetMyFriendsFailed(reason);
            return;
        }

        emit GetMyFriendsSucceeded(resp);

    },timeoutMs);



}
