#include "clientsocket.h"
#include "../utils/comapi/unit.h"

#include "../utils/appconfig.h"
ClientSocket::ClientSocket(QObject *parent)
    : QObject{parent}
{

    m_tcpSocket = new QTcpSocket(this);

    connect(m_tcpSocket,&QTcpSocket::readyRead,this,&ClientSocket::SltReadyRead);
    connect(m_tcpSocket,&QTcpSocket::connected,this,&ClientSocket::SltConnected);
    connect(m_tcpSocket,&QTcpSocket::disconnected,this,&ClientSocket::SltDisconnected);
}

ClientSocket::~ClientSocket()
{
    SltSendOffline();
}

QString ClientSocket::GetUserId() const
{
    return m_nId;
}

void ClientSocket::SetUserId(const QString &id)
{
    m_nId = id;
}

void ClientSocket::CheckConnected()
{
    if(m_tcpSocket->state()!=QTcpSocket::ConnectedState){
        m_tcpSocket->connectToHost(AppConfig::instance().getFileHost(), AppConfig::instance().getPort());
    }
}

void ClientSocket::CloseConnected()
{
    if(m_tcpSocket->isOpen()){
        m_tcpSocket->abort();
    }
}

void ClientSocket::ConnectToHost(const QString &host, const int &port)
{
    if(m_tcpSocket->isOpen()) m_tcpSocket->abort();;
    m_tcpSocket->connectToHost(host,port);
}

void ClientSocket::ConnectToHost(const QHostAddress &host, const int &port)
{
    if(m_tcpSocket->isOpen()) m_tcpSocket->abort();;
    m_tcpSocket->connectToHost(host,port);
}

// 登录链接
void ClientSocket::connectToHostAsync()
{
    m_tcpSocket->connectToHost(AppConfig::instance().getHost(), AppConfig::instance().getPort());
}


// 新增消息发送方法
void ClientSocket::sendJsonMessage(const quint8 &type, const QJsonValue &dataVal)
{
    QJsonObject json;
    // 构建Json 对象

    json.insert("type",type);
    json.insert("from",GetUserId());
    json.insert("data",dataVal);

    // 构建 json 文档
    QJsonDocument document;
    document.setObject(json);

    //参数 QJsonDocument::Compact 表示“紧凑”格式：序列化后的 JSON 字符串不会包含多余的空格和换行，适合网络传输。
    qDebug() << "m_tcpsocket-write:" << document.toJson(QJsonDocument::Compact);

    m_tcpSocket->write(document.toJson(QJsonDocument::Compact));
}


void ClientSocket::SltSendMessage(const quint8 &type, const QJsonValue &dataVal)
{

    // 状态检查移到外部，避免重复连接
    if (m_tcpSocket->state() != QAbstractSocket::ConnectedState) {
        // 缓存消息到队列
        m_msgQueue.enqueue(qMakePair(type, dataVal));

        // 仅在未连接时发起连接
        if (m_tcpSocket->state() == QAbstractSocket::UnconnectedState) {
            connectToHostAsync(); // 异步连接
        }
        qDebug() << "SltSendMessage ::已设置检查和连接";
        return;
    }

    // 已连接状态直接发送
    sendJsonMessage(type, dataVal);

}

void ClientSocket::SltSendOnline()
{

}

void ClientSocket::SltSendOffline()
{
    QJsonObject m_json;
}

void ClientSocket::SltDisconnected()
{
    m_msgQueue.clear(); // 清空未发送消息

    if(m_tcpSocket->isOpen()) m_tcpSocket->close();
}

void ClientSocket::SltConnected()
{
    qDebug() << "Connected to server";

    // 发送所有缓存消息
    while (!m_msgQueue.isEmpty()) {
        auto msg = m_msgQueue.dequeue();
        sendJsonMessage(msg.first, msg.second);
    }

    // 可选：发送上线通知
    SltSendOnline();
    emit signalStatus(ConnectedHost);
}

void ClientSocket::SltReadyRead()
{
    // 读取 socket 数据
    QByteArray byRead = m_tcpSocket->readAll();

    // 解析为 JSON 文档
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(byRead, &jsonError);

    // 判断解析是否成功
    if (!document.isNull() && jsonError.error == QJsonParseError::NoError) {
        if (document.isObject()) {
            QJsonObject jsonObj = document.object();
            qDebug() << "[网络消息] 返回内容为:" << document;

            QJsonValue dataVal = jsonObj.value("data");

            int nType = jsonObj.value("type").toInt();

            emit signalMessage(nType,dataVal);
        }
    } else {
        qDebug() << "[JSON 解析错误] 错误原因:" << jsonError.errorString();
    }
}
