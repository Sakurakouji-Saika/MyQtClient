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

    if (role == Qt::DisplayRole || role == Qt::UserRole) {
        return message.text;
    } else if (role == Qt::UserRole + 1) {
        return message.isSelf;
    }

    return QVariant();
}

void ChatModel::addMessage(const QString &text, bool isSelf) {
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages.append({text, isSelf});
    endInsertRows();
}

void ChatModel::clear() {
    beginResetModel();
    m_messages.clear();
    endResetModel();
}
