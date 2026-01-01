#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include <QObject>


class PacketProcessor;
class SocketClient;
class AuthService;
class AvatarService;
class FriendService;


class ServiceManager : public QObject
{
    Q_OBJECT
public:
    explicit ServiceManager(QObject *parent = nullptr);
    ~ServiceManager() override;

    void init();
    bool start();
    void stop();

    bool isStarted() const; // 是否已连接


    SocketClient* socket() const { return m_socket; }
    PacketProcessor* processor() const { return m_processor; }


    AuthService* auth() const { return m_auth; }
    AvatarService  *avatar() const { return m_as; }
    FriendService *friendApi() const {return m_fs; }


private:

    bool m_started = false;

    SocketClient* m_socket = nullptr;
    PacketProcessor* m_processor = nullptr;
    AuthService* m_auth = nullptr;
    AvatarService *m_as = nullptr;
    FriendService *m_fs = nullptr;

};


#endif // SERVICEMANAGER_H
