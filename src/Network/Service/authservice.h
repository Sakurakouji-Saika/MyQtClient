#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include <QObject>
#include <QPointer>

class PacketProcessor;

class AuthService : public QObject
{
    Q_OBJECT
public:
    explicit AuthService(PacketProcessor* processor,QObject *parent = nullptr);
    ~AuthService() override;

    void login(const QString &username,const QString &password,int timeoutMs = 5000);
    void registration(const QString &username,const QString &password,const QString &email,int timeoutMs = 5000);

    void GetMyFriends(qint64 &id,int timeoutMs = 5000);


signals:

    void registrationSucceede(qint64 userId);
    void regostrationFailed(const QString &err);

    void loginSucceeded(QJsonObject user);
    void loginFailed(const QString &reason);

    void GetMyFriendsSucceeded(QJsonObject friends);
    void GetMyFriendsFailed(const QString &reason);


private:
    QPointer<PacketProcessor> m_processor;


signals:
};

#endif // AUTHSERVICE_H
