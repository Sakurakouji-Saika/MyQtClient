#include "avatarservice.h"
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

#include "../PacketProcessor/packetprocessor.h"

#include "../../src/utils/comapi/Protocol.h"
#include "../Files/tempchunkmanager.h"
#include "../../utils/appconfig.h"
#include "../../DataBaseManage/databasemanage.h"
#include <QMessageBox>

#include "../../src/widgets/avatar/avatarmanager.h"


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

void AvatarService::UpoadLoadAvatarStart(const QString &fullFilePath)
{
    QJsonObject request;
    request["type"] = static_cast<int>(Protocol::MessageType::SendImageStart);

    if(!m_pp){
        qDebug() << "AvatarService::UpoadLoadAvatarStart::不存在";
        return;
    }

    QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);  // 生成一个唯一的 UUID 用于下载会话
    const qint64 CHUNK_SIZE = 64 * 1024;                                // 每块64KB

    QByteArray fileData;
    qint64 chunk_num;


    QString path = QDir::fromNativeSeparators(fullFilePath); // 把所有 '\\' 转成 '/'
    QFileInfo fi(path);

    avatarTempFileUploadPath = fi.fileName();


    if(!readAvatarFile(fullFilePath,fileData,CHUNK_SIZE,chunk_num)){
        qDebug() << "AvatarService::UpoadLoadAvatarStart::文件读取失败";
        return;
    }

    request["filename"] = fi.fileName();
    request["uuid"] = uuid;
    request["chunk_size"] = CHUNK_SIZE;
    request["chunk_num"] = chunk_num;
    request["file_size"] = fileData.size();

    m_pp->sendJson(request);

    UpoadLoadAvatarChunk(uuid,fullFilePath,fileData,CHUNK_SIZE,chunk_num);

}

void AvatarService::UpoadLoadAvatarChunk(const QString &uuid, const QString &fileName, QByteArray &fileData, const qint64 &chunk_size,const qint64 &chunk_num)
{
    QJsonObject chunkObj;
    chunkObj["type"] = static_cast<int>(Protocol::MessageType::SendImageChunk);

    for(int chunk_id =0 ;chunk_id < chunk_num; ++chunk_id){
        chunkObj["chunk_id"] = QString::number(chunk_id);

        qint64 offset = chunk_id * chunk_size;
        qint64 len = qMin(chunk_size,(qint64)fileData.size() - offset);
        QByteArray chunk = fileData.mid(offset,len);

        // 编码为 Base64

        chunkObj["chunk_size"] = QString::number(len);
        chunkObj["uuid"] = uuid;
        chunkObj["content"] = QString::fromUtf8(chunk.toBase64());

        if (chunk_id == chunk_num - 1) {
            chunkObj["is_last"] = true;
        }else{
            chunkObj["is_last"] = false;
        }
    }

    m_pp->sendJson(chunkObj);
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

void AvatarService::avatarUploadSucceeded(const QJsonObject &packet)
{
    qDebug() << "AvatarService::avatarUploadSucceeded::" <<  packet;

    QMessageBox::information(nullptr,tr("提示"),tr("头像上传成功"));

    QString oldPath = AppConfig::instance().imagesDirectory() + QDir::separator() + avatarTempFileUploadPath;
    QString newPath = AppConfig::instance().imagesDirectory() + QDir::separator() + packet.value("file_name").toString();

    qint64 uid = packet.value("uid").toDouble();
    qint64 file_id = packet.value("file_id").toString().toLongLong();
    QString file_name = packet.value("file_name").toString();

    if (!QFile::rename(oldPath, newPath)) {
        QMessageBox::information(nullptr,tr("提示"),tr("修改本地临时上传头像文件名失败！"));
    }

    qDebug() << "更新用户头像信息"
             << "\t uid:" << uid
             << "\t file_id" << file_id
             << "\t file_name" << file_name;

    if(!DataBaseManage::instance()->updateUserAvatarById(uid,file_id,file_name)){

    }

    AvatarManager::instance().avatarUpdated(uid,file_name);

}

void AvatarService::avatarUploadFailed(const QJsonObject &packet)
{
    QMessageBox::information(nullptr,
                             tr("提示"),
                             tr("头像上传失敗"));
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

bool AvatarService::readAvatarFile(const QString &fullFilePath, QByteArray &fileData, const qint64 &chunk_size, qint64 &chunk_num)
{

    QFile f(fullFilePath);

    if(!f.exists()){
        QMessageBox MyBox(QMessageBox::Question,"上传头像失败","要修改的临时头像文件不存在",QMessageBox::No);
        //使 MyBox 对话框显示
        MyBox.exec();
        return false;
    }

    if(!f.open(QIODevice::ReadOnly)){
        QMessageBox MyBox(QMessageBox::Question,"上传头像失败","无法读取临时头像文件数据",QMessageBox::No);
        //使 MyBox 对话框显示
        MyBox.exec();
        return false;
    }

    fileData = f.readAll();
    f.close();

    chunk_num = (fileData.size() + (chunk_size - 1)) / chunk_size;    // 每块64KB
    return true;
}

