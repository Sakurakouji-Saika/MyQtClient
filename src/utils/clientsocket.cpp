#include "clientsocket.h"
#include "comapi/unit.h"
#include "comapi/myapp.h"

ClientSocket::ClientSocket(QObject *parent)
    : QObject{parent}
{
    m_nId = -1;

    m_tcpSocket = new QTcpSocket(this);

    connect(m_tcpSocket,&QTcpSocket::readyRead,this,&ClientSocket::SltReadyRead);
    connect(m_tcpSocket,&QTcpSocket::connected,this,&ClientSocket::SltConnected);
    connect(m_tcpSocket,&QTcpSocket::disconnected,this,&ClientSocket::SltDisconnected);
}

ClientSocket::~ClientSocket()
{
    SltSendOffline();
}

int ClientSocket::GetUserId() const
{
    return m_nId;
}

void ClientSocket::SetUserId(const int &id)
{
    m_nId = id;
}

void ClientSocket::CheckConnected()
{
    if(m_tcpSocket->state()!=QTcpSocket::ConnectedState){
        m_tcpSocket->connectToHost(MyApp::m_strHostAddr, MyApp::m_nMsgPort);
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
    m_tcpSocket->connectToHost(MyApp::m_strHostAddr, MyApp::m_nMsgPort);
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
            qDebug() << "[登陆消息] 返回内容为:" << document;

            QJsonValue dataVal = jsonObj.value("data");
            int nFrom = jsonObj.value("from").toInt();
            int nType = jsonObj.value("type").toInt();

            switch(nType){
            case Register:
                //注册
                break;
            case Login:  //登录 编号17
                ParseLogin(dataVal);
                break;
            }

        }
    } else {
        qDebug() << "[JSON 解析错误] 错误原因:" << jsonError.errorString();
    }
}


// 用户登录
void ClientSocket::ParseLogin(const QJsonValue &dataVal)
{

    QJsonObject dataObj = dataVal.toObject();
    int id = dataObj.value("id").toInt();
    int code = dataObj.value("code").toInt();
    QString msg = dataObj.value("msg").toString();
    if(id == -1){
        qDebug() << "[登录信息]:" << "用户未注册";
        emit signalStatus(LoginPasswdError);
    }else if(id == -2){
        qDebug() << "[登录信息]:" << "用户已在线";
        emit signalStatus(LoginRepeat);
        SetUserId(id);
    }else if(id > 0 && code ==0 && msg =="ok"){
        qDebug() << "[登录信息]:" << "用户成功";
        emit signalStatus(LoginSuccess);
    }else{
        qDebug() << "[登录信息]:" << "未知错误";
        emit signalStatus(0x01);    //这个0x01 是我自己定得。不是文档得标准
    }


}

void ClientSocket::ParseReister(const QJsonValue &dataVal)
{

}
