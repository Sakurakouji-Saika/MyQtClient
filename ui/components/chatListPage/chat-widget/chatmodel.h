#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QAbstractListModel>
#include <QStringList>

struct MessageData {
    QString text;
    bool isSelf;
    QString avatar_url;
};


enum ChatRoles {
    TextRole = Qt::UserRole,       // 消息文本
    IsSelfRole,                     // 是否自己发的
    avatarUrlRole                   // 头像地址

};

class ChatModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit ChatModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addMessage(const QString &text, bool isSelf,QString avatar_url);
    void clear();

private:
    QList<MessageData> m_messages;
};

#endif // CHATMODEL_H
