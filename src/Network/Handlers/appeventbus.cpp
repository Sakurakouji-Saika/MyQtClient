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
