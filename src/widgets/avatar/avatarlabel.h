#ifndef AVATARLABEL_H
#define AVATARLABEL_H

#include <QLabel>
#include <QObject>

class AvatarLabel : public QLabel {
    Q_OBJECT
public:
    explicit AvatarLabel(QWidget *parent = nullptr);
    void setUserId(qint64 userId);

private slots:
    void onAvatarUpdated(qint64 userId, const QString &localPath);

private:
    void loadLocalAvatar(const QString &localPath);

    qint64 m_userId = -1;
};

#endif // AVATARLABEL_H
