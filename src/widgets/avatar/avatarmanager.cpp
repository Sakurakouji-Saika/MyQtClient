#include "avatarmanager.h"

#include <QReadLocker>
#include <QWriteLocker>

AvatarManager &AvatarManager::instance()
{
    static AvatarManager inst;
    return inst;
}

AvatarManager::AvatarManager(QObject *parent)
    : QObject(parent)
{
}

AvatarManager::~AvatarManager()
{
    QWriteLocker locker(&m_lock);
    m_map.clear();
}

QString AvatarManager::avatarUrl(qint64 userId) const
{
    QReadLocker locker(&m_lock);
    return m_map.value(userId);
}

void AvatarManager::updateAvatar(qint64 userId, const QString &localPath)
{
    {
        QWriteLocker locker(&m_lock);
        m_map.insert(userId, localPath);
    }

    emit avatarUpdated(userId, localPath);
}
