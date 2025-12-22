#include "handlerregistry.h"


#include "../Service/servicemanager.h"
#include "../Service/avatarservice.h"
#include "../PacketProcessor/packetprocessor.h"
#include "../../Src/utils/comapi/Protocol.h"


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


}

void handlerregistry::registerHandler(int messageType, JsonHandler handler)
{
    if(m_sm){
        m_sm->processor()->registerHandler(messageType,handler);
    }
}
