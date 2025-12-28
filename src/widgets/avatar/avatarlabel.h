#ifndef AVATARLABEL_H
#define AVATARLABEL_H

#include <QLabel>
#include <QObject>

class AvatarLabel : public QLabel {
    Q_OBJECT
public:
    explicit AvatarLabel(QWidget *parent = nullptr);
    void setAvatar(qint64 userId,const int window_size);

private slots:
    void onAvatarUpdated(qint64 userId, const QString &localPath);

private:
    void loadLocalAvatar(const QString &localPath);

    qint64 m_userId = -1;
    int m_window_size = 40;   // 默認窗口大小
};

#endif // AVATARLABEL_H
