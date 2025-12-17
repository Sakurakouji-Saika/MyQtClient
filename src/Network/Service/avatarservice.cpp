#include "avatarservice.h"
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

#include "../PacketProcessor/packetprocessor.h"
#include "../../Src/utils/comapi/Protocol.h"
#include "../Files/tempchunkmanager.h"

AvatarService::AvatarService(PacketProcessor *processor,QObject *parent)
    : QObject{parent},
    m_pp{processor}
{
    m_tcm = new TempChunkManager();
}


// 主动请求
void AvatarService::requestAvatarById(QString file_avatarID)
{
    if(!m_pp){
        emit requestAvatarByIdFailed(QStringLiteral("AvatarService::requestAvatarById:: 包处理器不存在"));
        return;
    }

    QJsonObject request;
    request["type"] = static_cast<int>(Protocol::MessageType::ReceiveImageStart);
    request["file_id"] = file_avatarID;

    m_pp->sendJson(request);



}

void AvatarService::DownloadAvatarStart(const QJsonObject &packet)
{
    qDebug() << "AvatarService::DownloadAvatarStart::" << packet;

    QString filename = packet["filename"].toString();
    QString uuid     = packet["uuid"].toString();

    qint64 chunkSize = packet["file_chunk_size"].toVariant().toLongLong();
    qint64 chunkNum  = packet["file_chunk_num"].toVariant().toLongLong();

    QString fileId   = packet["file_id"].toString();
    qint64 fileSize  = packet["file_size"].toVariant().toLongLong();

    int code = packet["code"].toInt();
    int type = packet["type"].toInt();

    if (filename.isEmpty() || uuid.isEmpty() || chunkSize <= 0 || chunkNum <= 0) {
        qDebug() << "AvatarService::DownloadAvatarStart:: 无效参数.";
        return;
    }

    QString error;
    if (!m_tcm->createSession(uuid,filename,chunkSize,chunkNum,error)){
        qDebug() << "AvatarService::DownloadAvatarStart::创建下载文件失败!";
        return;
    };
}

void AvatarService::DownloadAvatarChunk(const QJsonObject &packet)
{
    const qint64 CHUNK_SIZE = 64 * 1024; // 64KB


    int    type      = packet.value("type").toInt();
    QString uuid     = packet.value("uuid").toString();
    qint64 fileId    = packet.value("file_id").toString().toLongLong();
    qint64 chunkId   = packet.value("chunk_id").toString().toLongLong();
    qint64 chunkSize = packet.value("chunk_size").toString().toLongLong(); // 报告的分片长度（字符串或数字）
    bool   isLast    = packet.value("is_last").toBool();

    QByteArray contentBase64 = packet.value("content").toString().toUtf8();
    QByteArray content = QByteArray::fromBase64(contentBase64);

    if (uuid.isEmpty() || chunkId < 0 || contentBase64.isEmpty()) {
        qDebug() << "AvatarService::DownloadAvatarChunk :: 传递传输不正确!";
        return;
    }

    QString error;
    if (!m_tcm->addChunk(uuid, chunkId, content, error)) {
        qDebug() << "AvatarService::DownloadAvatarChunk ::[addChunk] :: 错误:" << error;
        return;
    }

    // 检查是否全部接收完毕
    if (m_tcm->isComplete(uuid)) {
        QByteArray fileData = m_tcm->getAndRemoveSession(uuid, error);
        if (fileData.isEmpty()) {
            qDebug() << "AvatarService::DownloadAvatarChunk ::[getAndRemoveSession] :: 错误:" << error;
            return;
        }

        QString filename = m_tcm->getFileName(uuid);

        if (filename.isEmpty()) {
            // 从 session 记录中读取（需要修改 TempChunkManager 提供该接口，暂时使用默认）
            filename = QString("avatar_%1.png").arg(QDateTime::currentSecsSinceEpoch());
        }

        // 保存文件到磁盘并插入 files 表
        if (!saveAvatarFile(fileId, filename, fileData, fileId, error)) {
            return;
        }
    }
}


bool AvatarService::saveAvatarFile(qint64 userId, const QString &filename, const QByteArray &data, qint64 &outFileId, QString &outError)
{
    // 获取存储目录
    QString dir = QCoreApplication::applicationDirPath() + QDir::separator() + "AvatarDir";
    QDir d;
    if (!d.exists(dir)) {
        if (!d.mkpath(dir)) {
            outError = "failed to create storage dir";
            return false;
        }
    }

    // 生成唯一文件名并保存
    QString uniqueName = QString("avatar_%1_%2_%3.png")
                             .arg(QString::number(userId))
                             .arg(QDateTime::currentSecsSinceEpoch())
                             .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
    QString fullPath = QDir(dir).filePath(uniqueName);

    QFile f(fullPath);
    if (!f.open(QIODevice::WriteOnly)) {
        outError = "failed to open file for writing";
        return false;
    }
    if (f.write(data) != data.size()) {
        f.close();
        QFile::remove(fullPath);
        outError = "write incomplete";
        return false;
    }
    f.close();


    // 插入 files 表
    // FileRepository frepo;
    // if (!frepo.createFile(userId, QString("avatar"), uniqueName, data.size(), outFileId, outError)) {
    //     QFile::remove(fullPath);
    //     return false;
    // }

    return true;
}

