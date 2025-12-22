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
    void requestAvatarById(QString file_avatarID);

    void RequestAvatarInfoByUserID(qint64 uid);


    void UpoadLoadAvatarStart(const QJsonObject &packet);

    void UpoadLoadAvatarChunk(const QJsonObject &packet);



    // 被动
    void DownloadAvatarStart(const QJsonObject &packet);

    void DownloadAvatarChunk(const QJsonObject &packet);

private:

    // 保存文件到磁盘并插入 files 表
    bool saveAvatarFile(qint64 userId, const QString &filename, const QByteArray &data, qint64 &outFileId, QString &outError);


signals:
    void requestAvatarByIdFailed(const QString &err);

    void avatarNicknameFetched(const qint64 uid,const qint64 file_id, const QString fileName);
    void avatarNicknameFetchFailed(const QString err);


private:

    QPointer<PacketProcessor> m_pp;
    TempChunkManager * m_tcm;
};

#endif // AVATARSERVICE_H
