#include "handlerregistry.h"


#include "../Service/servicemanager.h"
#include "../Service/avatarservice.h"
#include "../Service/friendservice.h"
#include "../PacketProcessor/packetprocessor.h"
#include "../../src/utils/comapi/Protocol.h"
#include "appeventbus.h"

handlerregistry::handlerregistry()
{

}

void handlerregistry::setServiceManager(ServiceManager *_sm)
{
    m_sm = _sm;
}

void handlerregistry::registerAll()
{
    registerHandler(static_cast<int>(Protocol::MessageType::ReceiveImageStart),[this](const QJsonObject& json){
        m_sm->avatar()->DownloadAvatarStart(json);
    });

    registerHandler(static_cast<int>(Protocol::MessageType::ReceiveImageChunk),[this](const QJsonObject& json){
        m_sm->avatar()->DownloadAvatarChunk(json);
    });


    registerHandler(static_cast<int>(Protocol::MessageType::avatarUploadSucceeded),[this](const QJsonObject& json){
        m_sm->avatar()->avatarUploadSucceeded(json);
    });

    registerHandler(static_cast<int>(Protocol::MessageType::avatarUploadFailed),[this](const QJsonObject& json){
        m_sm->avatar()->avatarUploadFailed(json);
    });

    registerHandler(static_cast<int>(Protocol::MessageType::avatarUploadFailed),[this](const QJsonObject& json){
        m_sm->avatar()->avatarUploadFailed(json);
    });


    // 广播好友上线事件
    registerHandler(static_cast<int>(Protocol::MessageType::FriendOnline),[this](const QJsonObject& json){
        m_sm->broadcastAPI()->friendStatusChanged(json);
    });

    // 广播好友下载线事件
    registerHandler(static_cast<int>(Protocol::MessageType::FriendOffline),[this](const QJsonObject& json){
        m_sm->broadcastAPI()->friendStatusChanged(json);
    });

    // 广播好友更新头像事件
    registerHandler(static_cast<int>(Protocol::MessageType::UpdateAvatarFriend),[this](const QJsonObject& json){
        m_sm->broadcastAPI()->UpdateAvatar(json);
    });

    // 广播被好友删除事件
    registerHandler(static_cast<int>(Protocol::MessageType::RemovedByFriend),[this](const QJsonObject& json){
        m_sm->broadcastAPI()->RemovedByFriend(json);
    });

    // 广播申请好友成功事件
    registerHandler(static_cast<int>(Protocol::MessageType::friendAddedByRequest),[this](const QJsonObject& json){
        m_sm->broadcastAPI()->friendAdded(json);
    });

    // 广播收到好友发来的消息
    registerHandler(static_cast<int>(Protocol::MessageType::SendText),[this](const QJsonObject& json){
        m_sm->broadcastAPI()->ReceiveMessage(json);
    });

}

void handlerregistry::registerHandler(int messageType, JsonHandler handler)
{
    if(m_sm){
        m_sm->processor()->registerHandler(messageType,handler);
    }
}
