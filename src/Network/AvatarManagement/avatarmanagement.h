#ifndef AVATARMANAGEMENT_H
#define AVATARMANAGEMENT_H

#include <QObject>
#include "../FileManager/filemanager.h"
#include "../../utils/comapi/unit.h"
#include "../../utils/appconfig.h"
#include <QTimer>

class AvatarManagement : public QObject
{
    Q_OBJECT
public:
    explicit AvatarManagement(QObject *parent = nullptr);

    // 下载头像
    void DownloadAvatar(const int &userId);

private slots:

    void onSendFinished();
    void onFileRecvOk(quint8 type, const QString &filePath);
    void onConnected();
    void onBeginToSend();

signals:

    // 头像上传完成
    void avatarUploadFinished(const int &userId, const QString &AvatarFilePath);

private:
    FileManager *m_uploadAvatarMgr;

    quint8  m_transferType;     // 当前操作类型
    qint32  m_targetUserId;     // 目标用户的 ID
};

#endif // AVATARMANAGEMENT_H
