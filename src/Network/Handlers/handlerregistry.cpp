#include "handlerregistry.h"


#include "../Service/servicemanager.h"
#include "../Service/avatarservice.h"
#include "../PacketProcessor/packetprocessor.h"
#include "../../src/utils/comapi/Protocol.h"


handlerregistry::handlerregistry(ServiceManager *_sm)
    :m_sm{_sm}
{}

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


}

void handlerregistry::registerHandler(int messageType, JsonHandler handler)
{
    if(m_sm){
        m_sm->processor()->registerHandler(messageType,handler);
    }
}
