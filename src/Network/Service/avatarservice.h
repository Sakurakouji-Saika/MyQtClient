#ifndef AVATARSERVICE_H
#define AVATARSERVICE_H

#include <QObject>
#include <QPointer>
#include <QString>
#include <QJsonObject>


class PacketProcessor;
class TempChunkManager;

class AvatarService : public QObject
{
    Q_OBJECT
public:
    explicit AvatarService(PacketProcessor *processor,QObject *parent = nullptr);

    // 主动
    void requestAvatarByFileId(QString file_avatarID);

    void RequestAvatarInfoByUserID(qint64 uid);


    void UpoadLoadAvatarStart(const QString &fullFilePath);

    void UpoadLoadAvatarChunk(const QString &uuid, const QString &fullFilePath, QByteArray &fileData, const qint64 &chunk_size,const qint64 &chunk_num);



    // 被动
    void DownloadAvatarStart(const QJsonObject &packet);

    void DownloadAvatarChunk(const QJsonObject &packet);

    void avatarUploadSucceeded(const QJsonObject &packet);

    void avatarUploadFailed(const QJsonObject &packet);

private:

    // 保存文件到磁盘并插入 files 表
    bool saveAvatarFile(qint64 userId, const QString &filename, const QByteArray &data, qint64 &outFileId, QString &outError);

    bool readAvatarFile(const QString &fileName, QByteArray &fileData, const qint64 &chunk_size, qint64 &chunk_num);


signals:
    void requestAvatarByIdFailed(const QString &err);

    void avatarNicknameFetched(const qint64 uid,const qint64 file_id, const QString fileName);
    void avatarNicknameFetchFailed(const QString err);


private:

    QPointer<PacketProcessor> m_pp;
    TempChunkManager * m_tcm;

    QString avatarTempFileUploadPath;
};

#endif // AVATARSERVICE_H
