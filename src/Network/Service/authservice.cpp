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


    m_processor->sendRequest(req,[this,username](const QJsonObject &resp){
        bool ok = resp.value("ok").toBool(false);
        if(!ok){

            qDebug() << "登录消息:" << resp;
            QString reason = resp.value("error").toString("服务端错误信息提示为空");
            emit loginFailed(reason);
            return;
        }



        QString idStr = resp.value("user").toObject().value("id").toString();
        qint64 uid = idStr.toLongLong();
        m_userId = uid;
        m_username = username;

        emit loginSucceeded(uid);

    },timeoutMs);

}

void AuthService::logout()
{
    m_userId = -1;
    m_username.clear();
    emit loggedOut();
}

bool AuthService::isAuthenticated() const
{
    return m_userId != -1;
}

qint64 AuthService::userId() const
{
    return m_userId;
}
