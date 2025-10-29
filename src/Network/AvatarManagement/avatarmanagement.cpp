#include "avatarmanagement.h"

AvatarManagement::AvatarManagement(QObject *parent)
    : QObject{parent}
{
    m_transferType = Unknow;
    m_targetUserId = AppConfig::instance().getUserID().toInt();

    m_uploadAvatarMgr = new FileManager(this);
    // 启动的时候就连接服务器，方便头像下载
    m_uploadAvatarMgr->ConnectToServer(AppConfig::instance().getFileHost(),AppConfig::instance().getFilePort(), WINID_HEAD_PATH);

    connect(m_uploadAvatarMgr, &FileManager::signalSendFinished, this, &AvatarManagement::onSendFinished);
    connect(m_uploadAvatarMgr, &FileManager::signalConnected,    this, &AvatarManagement::onConnected);
    connect(m_uploadAvatarMgr, &FileManager::signalFileRecvOk,   this, &AvatarManagement::onFileRecvOk);

}

void AvatarManagement::DownloadAvatar(const int &userId)
{
    if (Unknow != m_transferType) return;

    m_transferType = GetFile;
    m_targetUserId = userId;

    m_uploadAvatarMgr->ConnectToServer(AppConfig::instance().getFileHost(),AppConfig::instance().getFilePort(), WINID_HEAD_PATH);
}

void AvatarManagement::onConnected()
{
    // 需要延迟1s左右上传，服务器更新较慢
    if (UpdateHeadPic == m_transferType){
        QTimer::singleShot(1000, this, [this]() {
            this->onBeginToSend();
        });
    }

    // 如果是获取文件操作，连接成功后向文件服务器请求目标用户的头像
    if (GetFile == m_transferType) {
        QString remoteName = QString::number(m_targetUserId) + "_avatar.png";
        qDebug() << "AvatarManagement: requesting avatar file:" << remoteName;
        if (m_uploadAvatarMgr->isConnection()) {
            m_uploadAvatarMgr->RequestFile(remoteName);
        } else {
            // 若尚未连接，在下一次 signalConnected 回调中会处理（FileManager::onConnected 会 emit signalConnected）
        }
    }
}

void AvatarManagement::onSendFinished()
{
    m_transferType = Unknow;
    // 可以发送信号，表示上传头像完成

}

/**
 * @brief AvatarManagement::onFileRecvOk
 * 文件接收完成
 */
void AvatarManagement::onFileRecvOk(quint8 type, const QString &filePath)
{

    m_transferType = Unknow;
    if (AppConfig::instance().getUserID() != m_targetUserId) {
        // 头像上传完成，更新头像
        emit avatarUploadFinished(m_targetUserId, filePath);
    }


}


/**
 * @brief AvatarManagement::onConnected
 * 链接服务器，等待上传
 */


/**
 * @brief AvatarManagement::onBeginToSend
 * 文件开始上传头像到服务器
 */
void AvatarManagement::onBeginToSend()
{
    m_uploadAvatarMgr->StartTransferFile(AppConfig::instance().getUserID()+"_avatar.png");
}
