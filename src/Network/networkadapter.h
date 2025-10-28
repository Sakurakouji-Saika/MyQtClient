#ifndef NETWORKADAPTER_H
#define NETWORKADAPTER_H

#include "inetwork.h"
#include "clientsocket.h" // 你现有的实现
#include <QJsonValue>

class NetworkAdapter : public INetwork
{
    Q_OBJECT
public:


public:
    explicit NetworkAdapter(ClientSocket *client = nullptr, QObject *parent = nullptr);
    ~NetworkAdapter() override;

    // INetwork 接口实现
    void sendMessage(quint8 type, const QJsonValue &dataVal) override;
    void checkConnected() override;
    void connectToHostAsync() override;
    void closeConnected() override;

    // 其他接口
    void setUserID(QString _id);
    ClientSocket* rawClientSocket() const { return m_client; }

private slots:
    void onClientStatus(const quint8 &state);
    void onClientMessage(const quint8 &type, const QJsonValue &dataVal);

private:
    ClientSocket *m_client = nullptr;
    bool m_ownsClient = false;
};

#endif // NETWORKADAPTER_H
