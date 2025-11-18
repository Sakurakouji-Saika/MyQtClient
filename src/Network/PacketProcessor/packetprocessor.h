#ifndef PACKETPROCESSOR_H
#define PACKETPROCESSOR_H

#include <QObject>
#include <QByteArray>
#include <QJsonObject>
#include <QMap>
#include <functional>


class SocketClient;

using JsonHandler = std::function<void(const QJsonObject&)>;

class PacketProcessor : public QObject
{
    Q_OBJECT
public:
    explicit PacketProcessor(SocketClient* _scoketClient,QObject *parent = nullptr);
    ~PacketProcessor() override;

    // 注册自定义处理器
    void registerHandler(int type,JsonHandler handler);
    void unregisterHandler(int type);


    void sendJson(const QJsonObject &obj);

signals:
    // 当解析不到 JSON（或业务想直接处理原始数据）时会发出
    void rawFrameReceived(const QByteArray &payload);

private slots:
    void onFrame(const QByteArray &payload);

private:
    SocketClient* m_net;
    QMap<int, JsonHandler> m_handlers;


signals:
};

#endif // PACKETPROCESSOR_H
