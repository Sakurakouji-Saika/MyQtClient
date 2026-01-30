#include "chatmodel.h"

ChatModel::ChatModel(QObject *parent)
    : QAbstractListModel(parent) {
}

int ChatModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return m_messages.size();
}

QVariant ChatModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_messages.size())
        return QVariant();

    const MessageData &message = m_messages.at(index.row());

    switch (role) {
    case TextRole:
        return message.text;
    case IsSelfRole:
        return message.isSelf;
    case avatarUrlRole:
        return message.avatar_url;
    default:
        return QVariant();
    }

    return QVariant();
}

void ChatModel::addMessage(const QString &text, bool isSelf,QString avatar_url) {
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages.append({text, isSelf,avatar_url});
    endInsertRows();
}

void ChatModel::addMessages(const QList<MessageData> &data)
{
    if(data.isEmpty()) return;

    const int oldSize = m_messages.size();
    const int addCount = data.size();

    beginInsertRows(QModelIndex(), oldSize, oldSize + addCount -1 );
    m_messages.reserve(oldSize + addCount);
    m_messages.append(data);
    endInsertRows();

}

void ChatModel::clear() {
    beginResetModel();
    m_messages.clear();
    endResetModel();
}
