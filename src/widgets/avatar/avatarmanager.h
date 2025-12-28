#ifndef AVATARMANAGER_H
#define AVATARMANAGER_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QReadWriteLock>

class AvatarManager : public QObject
{
    Q_OBJECT
public:
    ~AvatarManager();

    static AvatarManager &instance();

    QString avatarUrl(qint64 userId) const;
    void updateAvatar(qint64 userId, const QString &localPath);



signals:

    void avatarUpdated(qint64 userId, const QString &newLocalPath);

private:
    explicit AvatarManager(QObject *parent = nullptr);


    mutable QReadWriteLock m_lock;
    QHash<qint64, QString> m_map;
};


#endif // AVATARMANAGER_H
