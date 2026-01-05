#include "avatarlabel.h"


#include "avatarmanager.h"
#include <QPixmap>
#include <QPixmapCache>
#include <QFile>
#include <QImageReader>
#include <QPainter>
#include <QPainterPath>
#include <QDir>
#include "../../src/utils/appconfig.h"
#include "../../src/DataBaseManage/databasemanage.h"

AvatarLabel::AvatarLabel(QWidget *parent)
    : QLabel(parent)
{
    // 订阅 AvatarManager 的广播（QueuedConnection 跨线程安全）
    connect(&AvatarManager::instance(), &AvatarManager::signalsAvatarUpdated,
            this, &AvatarLabel::onAvatarUpdated, Qt::QueuedConnection);

    setScaledContents(false);
    setAlignment(Qt::AlignCenter);
}

void AvatarLabel::setAvatar(qint64 userId,const int window_size) {

    m_window_size = window_size;
    m_userId = userId;

    QString url = AvatarManager::instance().avatarUrl(userId);

    qDebug() << "AvatarLabel::setUserId:"<< userId <<"\t url:" << url;

    if (!url.isEmpty()){
        loadLocalAvatar(url);
        return;
    }
}

void AvatarLabel::onAvatarUpdated(qint64 userId, const QString &localPath) {
    if (userId != m_userId) return;

    // // 判断文件存不存在.
    // QString fileName = AppConfig::instance().imagesDirectory() + QDir::separator() + localPath;
    // QFileInfo fileInfo(fileName);


    loadLocalAvatar(localPath);
    qDebug() << "AvatarLabel::onAvatarUpdated::localPath" << localPath;

}

void AvatarLabel::loadLocalAvatar(const QString &localPath) {

    if (localPath.isEmpty()) {
        clear();
        return;
    }

    QPixmap src(AppConfig::instance().imagesDirectory() + QDir::separator() +  localPath);

    const int size = m_window_size;
    QPixmap scaled = src.scaled(size, size,
                                Qt::KeepAspectRatioByExpanding,
                                Qt::SmoothTransformation);

    QPixmap result(size, size);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath path;
    path.addEllipse(0, 0, size, size);
    painter.setClipPath(path);

    painter.drawPixmap(0, 0, scaled);
    painter.end();

    setPixmap(result);

}
