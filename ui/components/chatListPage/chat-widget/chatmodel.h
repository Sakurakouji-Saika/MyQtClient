#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QAbstractListModel>
#include <QStringList>

struct MessageData {
    QString text;
    bool isSelf;
};

class ChatModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit ChatModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addMessage(const QString &text, bool isSelf);
    void clear();

private:
    QList<MessageData> m_messages;
};

#endif // CHATMODEL_H
