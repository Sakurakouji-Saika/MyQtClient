#include "tempchunkmanager.h"

#include <QDateTime>

TempChunkManager::TempChunkManager()
{}


bool TempChunkManager::createSession(const QString &uuid, const QString &filename, qint64 chunkSize, qint64 chunkNum, QString &outError)
{
    QMutexLocker lock(&m_mutex);

    if(m_sessions.contains(uuid)){
        outError = "File upload session already exists.";
        return false;
    }

    if(chunkSize <=0 || chunkNum <= 0){
        outError = "The file upload parameters are incorrect.";
        return false;
    }

    TempChunk t;
    t.chunkNum = chunkNum;
    t.chunkSize = chunkSize;
    t.filename = filename;
    t.uuid = uuid;
    t.createdTime = QDateTime::currentSecsSinceEpoch();
    t.receivedChunks = 0;
    m_sessions[uuid] = t;

    return true;
}



bool TempChunkManager::addChunk(const QString &uuid, qint64 chunkId, const QByteArray &data, QString &outError)
{
    QMutexLocker lock(&m_mutex);

    if(!m_sessions.contains(uuid)){
        outError = "File upload session not found.";
        return false;
    }

    TempChunk &t = m_sessions[uuid];
    if(chunkId < 0 || chunkId >= t.chunkNum){
        outError = "File upload Chunk ID error.";
        return false;
    }

    if(t.chunks.contains(chunkId)){
        outError = "File upload ID block duplicate.";
        return false;
    }

    t.chunks[chunkId] = data;
    t.receivedChunks++;
    return true;
}



bool TempChunkManager::isComplete(const QString &uuid)
{
    QMutexLocker locker(&m_mutex);
    auto it = m_sessions.find(uuid);  // 只查找一次
    if (it == m_sessions.end()) return false;

    return it->receivedChunks == it->chunkNum;
}



QByteArray TempChunkManager::getAndRemoveSession(const QString &uuid, QString &outError)
{
    QMutexLocker locker(&m_mutex);

    if(!m_sessions.contains(uuid)){
        outError = "The file upload UUID does not exist.";
        return QByteArray();
    }

    TempChunk &t = m_sessions[uuid];
    QByteArray result;

    if(t.chunkNum != t.receivedChunks){
        outError = "File upload not yet completed.";
        return QByteArray();
    }

    for(qint64 i=0; i < t.chunkNum; i++){
        if(!t.chunks.contains(i)){
            outError = "File chunk missing. [" + QString::number(i) + "]";
            return QByteArray();
        }
        result.append(t.chunks[i]);
    }

    m_sessions.remove(uuid);
    return result;
}



void TempChunkManager::removeSession(const QString &uuid)
{
    QMutexLocker locker(&m_mutex);
    m_sessions.remove(uuid);
}



void TempChunkManager::cleanupExpired(qint64 ttl)
{
    QMutexLocker locker(&m_mutex);

    qint64 now = QDateTime::currentSecsSinceEpoch();
    QStringList expiredSessionIds;

    for(auto it = m_sessions.begin(); it != m_sessions.end(); ++it){
        if(now - it->createdTime > ttl){
            expiredSessionIds.append(it.key());
        }
    }

    for(const QString &uuid:expiredSessionIds){
        m_sessions.remove(uuid);
    }


}

QString TempChunkManager::getFileName(const QString &uuid)
{
    return m_sessions[uuid].filename;
}

