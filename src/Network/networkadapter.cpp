#include "NetworkAdapter.h"

NetworkAdapter::NetworkAdapter(ClientSocket *client, QObject *parent)
    : INetwork(parent)
{
    if (client) {
        m_client = client;
        m_ownsClient = false;
    } else {
        m_client = new ClientSocket(this);
        m_ownsClient = true;
    }

    // 转发 ClientSocket 信号到 INetwork 信号
    connect(m_client, &ClientSocket::signalStatus,
            this, &NetworkAdapter::onClientStatus);

    connect(m_client, &ClientSocket::signalMessage,
            this, &NetworkAdapter::onClientMessage);

    m_client->CheckConnected();
}

NetworkAdapter::~NetworkAdapter()
{
    if (m_ownsClient && m_client) {
        m_client->deleteLater();
        m_client = nullptr;
    }
}

void NetworkAdapter::sendMessage(quint8 type, const QJsonValue &dataVal)
{
    if (!m_client) return;
    m_client->SltSendMessage(type, dataVal);
}

void NetworkAdapter::checkConnected()
{
    if (m_client) m_client->CheckConnected();
}

void NetworkAdapter::connectToHostAsync()
{
    if (m_client) m_client->connectToHostAsync();
}

void NetworkAdapter::closeConnected()
{
    if (m_client) m_client->CloseConnected();
}

void NetworkAdapter::setUserID(QString _id)
{
    m_client->SetUserId(_id);
}

void NetworkAdapter::onClientStatus(const quint8 &state)
{
    emit statusChanged(state);
}

void NetworkAdapter::onClientMessage(const quint8 &type, const QJsonValue &dataVal)
{
    qDebug() << "NetworkAdapter::onClientMessage::[type]:" << type;
    emit messageReceived(type, dataVal);
}
