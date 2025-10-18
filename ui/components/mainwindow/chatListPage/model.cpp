#include "model.h"

Model::Model(QObject *parent)
    : QAbstractListModel{parent}
{}

void Model::addItem(const Recent_Data &item)
{
    const int newRow =  m_list_data.size();
    beginInsertRows(QModelIndex(),newRow,newRow);
    m_list_data.append(item);
    endInsertRows();

    my_diy_sort(true);
}

int Model::rowCount(const QModelIndex &parent) const
{
    return m_list_data.size();
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) return{};

    int row = index.row();


    if(row < 0 || row >= m_list_data.size()) return QVariant();

    const Recent_Data & it = m_list_data.at(row);

    switch(role){

        case avatar_Role:       return it.avatarPath;
        case msg_Role:          return it.msg;
        case user_id_Role:      return it.user_id;
        case user_name_Role:    return it.userName;
        case msg_time_Role:     return it.msg_time;
        case timestamp_Role:    return it.timestamp;
        case unread_count_Role: return it.UnreadCount;
        case all_data_Role:     return QVariant::fromValue(it);
        case Qt::SizeHintRole:  return QSize(0, 72);    // 宽度视图会自动计算，这里高度固定 72

        default:                return {};

    }
}

bool Model::removeRow(int row)
{
    if (row < 0 || row >= m_list_data.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    m_list_data.removeAt(row);
    endRemoveRows();
    return true;
}

Recent_Data Model::get_Row(int row)
{
    return m_list_data[row];
}

void Model::my_diy_sort(bool descending)
{
    if(m_list_data.isEmpty()) return;

    beginResetModel();

    std::sort(m_list_data.begin(), m_list_data.end(), [descending](const Recent_Data &a, const Recent_Data &b) {
        // bool AUnRead = a.UnreadCount > 0;
        // bool BUnRead = b.UnreadCount > 0;

        // if (AUnRead != BUnRead) {
        //     return AUnRead && !BUnRead; // 未读在前
        // }

        // 相同 unread 状态，按时间排序
        if (descending) {
            return a.timestamp > b.timestamp; // 新 -> 旧
        } else {
            return a.timestamp < b.timestamp; // 旧 -> 新
        }
    });

    // 通知视图刷新
    endResetModel();



}

void Model::update_Item_At(int row, const Recent_Data &newData)
{
    if (row < 0 || row >= m_list_data.size()) return;
    m_list_data[row] = newData;
}

void Model::addItemFront(const Recent_Data &data)
{
    // 1️⃣ 先判断是否已存在该 user_id
    for (int i = 0; i < m_list_data.size(); ++i) {
        if (m_list_data[i].user_id == data.user_id) {
            m_list_data[i] = data;
            // 发出 dataChanged 通知（而不是插入）
            QModelIndex idx = index(i, 0);
            return;
        }
    }

    // 2️⃣ 不存在则真正插入
    beginInsertRows(QModelIndex(), 0, 0);
    m_list_data.insert(0, data);
    endInsertRows();
}
