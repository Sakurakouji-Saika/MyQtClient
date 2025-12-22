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
#include "../../utils/AppConfig.h"
#include "../../DataBaseManage/databasemanage.h"

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

void AvatarService::RequestAvatarInfoByUserID(qint64 uid)
{
    if(!m_pp){
        emit requestAvatarByIdFailed(QStringLiteral("AvatarService::RequestAvatarInfoByUserID:: 包处理器不存在"));
        return;
    }

    QJsonObject request;
    request["type"] = static_cast<int>(Protocol::MessageType::AvatarFilename);
    request["user_id"] = uid;




    m_pp->sendRequest(request,[this](const QJsonObject &resp){
        bool ok = resp["ok"].toBool();

        if(ok){

            qDebug() << "AvatarService::RequestAvatarInfoByUserID::resp:: " << resp << "\n";

            qint64 user_id = resp.value("user_id").toDouble();
            qint64 file_id = resp.value("file_id").toDouble();
            QString fileName = resp.value("path").toString();



            emit avatarNicknameFetched(user_id,file_id,fileName);
            return;

        }else{

            QString error = resp.value("error").toString();
            emit avatarNicknameFetchFailed(error);
            return;

        }
    });
}

void AvatarService::UpoadLoadAvatarStart(const QJsonObject &packet)
{

}

void AvatarService::UpoadLoadAvatarChunk(const QJsonObject &packet)
{

}

void AvatarService::DownloadAvatarStart(const QJsonObject &packet)
{



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


    int     type        = packet.value("type").toInt();
    QString uuid        = packet.value("uuid").toString();
    qint64  fileId      = packet.value("file_id").toString().toLongLong();
    qint64  chunkId     = packet.value("chunk_id").toString().toLongLong();
    qint64  chunkSize   = packet.value("chunk_size").toString().toLongLong(); // 报告的分片长度（字符串或数字）
    bool    isLast      = packet.value("is_last").toBool();

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

        QString outError;
        QString filename = m_tcm->getFileName(uuid,outError);

        QByteArray fileData = m_tcm->getAndRemoveSession(uuid, error);
        if (fileData.isEmpty()) {
            qDebug() << "AvatarService::DownloadAvatarChunk ::[getAndRemoveSession] :: 错误:" << error;
            return;
        }

        // 保存文件到磁盘并插入 files 表
        if (!saveAvatarFile(fileId, filename, fileData, fileId, error)) {
            return;
        }


    }
}


bool AvatarService::saveAvatarFile(qint64 fileid, const QString &filename, const QByteArray &data, qint64 &outFileId, QString &outError)
{
    // 获取存储目录
    QString dir = AppConfig::instance().imagesDirectory();
    QDir d;
    if (!d.exists(dir)) {
        if (!d.mkpath(dir)) {
            outError = "failed to create storage dir";
            return false;
        }
    }

    // // 生成唯一文件名并保存
    // QString uniqueName = QString("avatar_%1_%2_%3.jpg")
    //                          .arg(QString::number(fileid))
    //                          .arg(QDateTime::currentSecsSinceEpoch())
    //                          .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
    // QString fullPath = QDir(dir).filePath(uniqueName);


    QString fullPath = QDir(dir).filePath(filename);

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


    // 保存文件到数据库
    DataBaseManage::instance()->UpdateFriendAvatarByAvatarID(fileid,filename);

    return true;
}

