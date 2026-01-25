#include "appeventbus.h"
#include "../../DataBaseManage/databasemanage.h"

AppEventBus::AppEventBus(QObject *parent)
    : QObject{parent}
{

}

void AppEventBus::friendStatusChanged(const QJsonObject &json)
{
    qint64 uid = json["uid"].toInteger();
    int state = json["state"].toInteger();

    DataBaseManage *dbm = DataBaseManage::instance();
    dbm->updateFriendStateByUid(uid,state);

    emit Friend_OnlineSignal(uid,state);
}

void AppEventBus::messageReceived(const QJsonObject &json)
{

}

void AppEventBus::UpdateAvatar(const QJsonObject &json)
{
    qint64 uid     = json["uid"].toInteger();
    qint64 file_id = json["file_id"].toInteger();

    emit UpdateAvatarSignal(uid, file_id);
}

void AppEventBus::friendAdded(const QJsonObject &json)
{
    qDebug()<< "AppEventBus::friendAdded(const QJsonObject &json)::" << json;
    qint64 uid     = json["uid"].toInteger();

    QString avatar = json["avatar"].toString();
    qint64 avatar_file_id = json["avatar_file_id"].toInteger();
    QString nickname = json["nickname"].toString();
    QString username = json["username"].toString();
    int status = json["status"].toInteger();

    emit friendAddedSignal(uid,avatar,avatar_file_id,nickname,status,username);
}

void AppEventBus::RemovedByFriend(const QJsonObject &json)
{
    qint64 uid     = json["uid"].toInteger();

    emit RemovedByFriendSignal(uid);

}

void AppEventBus::ReceiveMessage(const QJsonObject &json)
{
    int msgType = json.value("msgType").toInteger();
    qint64 file_id = json.value("file_id").toInteger();
    qint64 send_at = json.value("send_at").toInteger();
    qint64 sender_id = json.value("sender_id").toInteger();
    QString msgContent = json.value("msgContent").toString();
    qint64 msgId = json.value("msgId").toInteger();
    qint64 receiver_id = json.value("receiver_id").toInteger();

    // 这里发送信号表示已接受，未完成
    emit ReceiveNewMsgSuccessSignals(msgId);

    // 这里发送信号表示已接受，未完成


    emit ReceiveNewMessageSignals(file_id,msgContent,msgType,send_at,sender_id,msgId,receiver_id);
}
