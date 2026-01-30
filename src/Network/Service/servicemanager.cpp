#include "servicemanager.h"
#include "../socketclient.h"
#include "authservice.h"
#include "friendservice.h"
#include "../PacketProcessor/packetprocessor.h"

#include "../../utils/appconfig.h"
#include "avatarservice.h"
#include "../Network/Handlers/appeventbus.h"

ServiceManager::ServiceManager(QObject *parent)
    : QObject{parent}
{

}

ServiceManager::~ServiceManager(){
    stop();
};

void ServiceManager::init()
{
    if(m_socket) return;


    m_socket = new SocketClient(this);
    m_processor = new PacketProcessor(m_socket,this);


    m_auth = new AuthService(m_processor,this);
    m_as = new AvatarService(m_processor,this);
    m_fs = new FriendService(m_processor,this);
    m_aeb = new AppEventBus();

}

bool ServiceManager::start()
{
    if(m_started) return true;
    if(!m_socket) init();

    m_socket->connectToServer(AppConfig::instance().getHost(),AppConfig::instance().getPort());

    qDebug()<< "ServiceManager::start() 触发";
    qDebug()<< "ServiceManager::start()::连接信息 [ip:" <<
        AppConfig::instance().getHost() << ":"
             << AppConfig::instance().getPort() <<  "];";
    qDebug()<< "`````````````````````````````````````````";

    connect(m_socket,&SocketClient::connected,this,[this](){
        m_started = true;

        qDebug()<< "ServiceManager::start()::连接成功;";
        qDebug()<< "ServiceManager::start()::连接信息 [ip:" <<
            AppConfig::instance().getHost() << ":"
                << AppConfig::instance().getPort() <<  "];";
    });

    connect(m_socket, &SocketClient::disconnected, this, [this]() {
        m_started = false;

        qDebug()<< "ServiceManager::start()::关闭链接;";
        qDebug()<< "ServiceManager::start()::关闭的主机信息 [ip:" <<
            AppConfig::instance().getHost() << ":"
                 << AppConfig::instance().getPort() <<  "];";

    });

    connect(m_socket, &SocketClient::errorOccurred, this, [this](const QString &err){
        qDebug()<< "ServiceManager::start()::链接失败.";
        qDebug()<< "ServiceManager::start()::链接失败的主机信息 [ip:" <<
            AppConfig::instance().getHost() << ":"
                 << AppConfig::instance().getPort() <<  "];";
    });


    return true;
}

void ServiceManager::stop()
{
    if (!m_socket && !m_processor && !m_auth && !m_as && !m_fs && !m_aeb) return;

        m_started = false;

    if (m_socket) {
        QObject::disconnect(m_socket, nullptr, this, nullptr);
        m_socket->closeConnection(false);
    }

    if (m_processor) {
        m_processor->clearPending();
    }

    if (m_auth) { m_auth->deleteLater(); m_auth = nullptr;}
    if (m_as) { m_as->deleteLater(); m_as = nullptr; }
    if (m_fs) { m_fs->deleteLater(); m_fs = nullptr; }
    if (m_processor) { m_processor->deleteLater(); m_processor = nullptr; }
    if (m_socket) { m_socket->deleteLater(); m_socket = nullptr; }
    if (m_aeb) { delete m_aeb; m_aeb = nullptr; }

    qDebug() << "ServiceManager::stop() 已清理资源";
}



bool ServiceManager::isStarted() const
{
    return m_started;
}
