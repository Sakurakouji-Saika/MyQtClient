#ifndef TEMPCHUNKMANAGER_H
#define TEMPCHUNKMANAGER_H

#include <QString>
#include <QMap>
#include <QByteArray>
#include <QMutex>
#include <cstdint>


// 临时块缓存：用于接收文件块并在全部收集后合并
class TempChunk
{
public:
    QString uuid;
    QString filename;
    qint64 chunkSize = 0;
    qint64 chunkNum = 0;
    qint64 receivedChunks = 0;
    QMap<qint64, QByteArray> chunks;  // chunkId -> content
    qint64 createdTime = 0;  // 创建时间戳（用于过期清理）
    qint64 usetID = -1;
};

class TempChunkManager
{
public:
    explicit TempChunkManager();

    // 创建一个新的上传会话
    bool createSession(const QString &uuid, const QString &filename, qint64 chunkSize, qint64 chunkNum, QString &outError);

    // 添加一个块到会话
    bool addChunk(const QString &uuid, qint64 chunkId, const QByteArray &data, QString &outError);

    // 检查会话是否完整（所有块都已收到）
    bool isComplete(const QString &uuid);

    // 获取完整的数据并删除会话
    QByteArray getAndRemoveSession(const QString &uuid, QString &outError);

    // 删除会话
    void removeSession(const QString &uuid);

    // 清理过期会话（超过 ttl 秒）
    void cleanupExpired(qint64 ttl = 3600);  // 默认 1 小时

    QString getFileName(const QString &uuid);

    void setUserID(const QString &uuid,const qint64 &id) { m_sessions[uuid].usetID  = id; }

    qint64 GetUserID(const QString &uuid) { return m_sessions[uuid].usetID; };

private:
    QMap<QString, TempChunk> m_sessions;
    QMutex m_mutex;


};

#endif // TEMPCHUNKMANAGER_H
