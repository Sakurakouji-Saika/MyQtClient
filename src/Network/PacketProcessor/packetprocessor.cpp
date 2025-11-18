#include "packetprocessor.h"

#include "../socketclient.h"
#include <QJsonParseError>
#include <QJsonDocument>

PacketProcessor::PacketProcessor(SocketClient *_scoketClient, QObject *parent)
    : QObject(parent), m_net(_scoketClient)
{
    if(m_net){
        connect(m_net,&SocketClient::frameReceived,this,&PacketProcessor::onFrame);
    };
}

PacketProcessor::~PacketProcessor()
{
    if(m_net) disconnect(m_net,&SocketClient::frameReceived,this,&PacketProcessor::onFrame);

}

void PacketProcessor::registerHandler(int type, JsonHandler handler)
{
    m_handlers[type] = std::move(handler);
}

void PacketProcessor::unregisterHandler(int type)
{
    m_handlers.remove(type);
}

void PacketProcessor::sendJson(const QJsonObject &obj)
{
    if(!m_net) return;
    m_net->sendJson(obj);
}

void PacketProcessor::onFrame(const QByteArray &payload)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(payload,&err);
    if( err.error == QJsonParseError::NoError && doc.isObject() ){
        QJsonObject obj = doc.object();

        if(!obj.contains("type")){
            qDebug() << "json 缺少数据类型" ;
            return;
        }

        int type = obj.value("type").toInt();

        if(m_handlers.contains(type)){
            try{
                m_handlers[type](obj);
            }catch(std::exception &e){
                qDebug() << "PacketProcessor::onFrame::处理异常：" << e.what();
            }
        }else{
            qDebug() << "未知类型：" << type << ", json:" << QJsonDocument(obj).toJson(QJsonDocument::Compact);
        }
    }else{
        // 让上层处理二进制
        emit rawFrameReceived(payload);
    }
}

