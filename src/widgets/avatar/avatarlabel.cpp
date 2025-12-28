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
    connect(&AvatarManager::instance(), &AvatarManager::avatarUpdated,
            this, &AvatarLabel::onAvatarUpdated, Qt::QueuedConnection);

    setScaledContents(false);
    setAlignment(Qt::AlignCenter);
}

void AvatarLabel::setAvatar(qint64 userId,const int window_size) {

    m_window_size = window_size;
    m_userId = userId;
    QString url = AppConfig::instance().imagesDirectory() + QDir::separator() +
        AvatarManager::instance().avatarUrl(userId);

    qDebug() << "AvatarLabel::setUserId:" << url;
    if (!url.isEmpty()) loadLocalAvatar(url);
}

void AvatarLabel::onAvatarUpdated(qint64 userId, const QString &localPath) {
    if (userId != m_userId) return;
    loadLocalAvatar(localPath);
}

void AvatarLabel::loadLocalAvatar(const QString &localPath) {
    if (localPath.isEmpty()) {
        clear();
        return;
    }


    QPixmap src(AppConfig::instance().imagesDirectory() + QDir::separator() +
                DataBaseManage::instance()->GetFriendAvatarById(AppConfig::instance().getUserID())->avatar);

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







    // // 2) 直接从文件按字节读取，避免某些平台/驱动的文件缓存问题
    // QFile f(localPath);
    // if (!f.open(QIODevice::ReadOnly)) {
    //     // 无法打开，显示默认或清空
    //     clear();
    //     return;
    // }
    // QByteArray data = f.readAll();
    // f.close();

    // QPixmap pix;
    // if (pix.loadFromData(data)) {
    //     // 可同时把渲染后的 pix 存入 QPixmapCache（以便复用）
    //     QPixmap scaled = pix.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    //     QPixmapCache::insert(scaled);
    //     setPixmap(scaled);
    // } else {
    //     clear();
    // }
}
