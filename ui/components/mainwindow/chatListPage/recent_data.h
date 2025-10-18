#ifndef RECENT_DATA_H
#define RECENT_DATA_H
#include <QString>
#include <QMetaType>
#include <QDateTime>


struct Recent_Data {
    QString avatarPath;         // 头像路径（可为空，表示使用占位图）
    QString msg;                // 当前最新消息
    QString user_id;            // 用户ID；
    QString userName;           // 用户名
    QDateTime msg_time;         // 发送时间
    qint64 timestamp;           // 发送时间戳
    int UnreadCount;            // 未读消息数量

    // 默认构造
    Recent_Data() = default;

    // 拷贝构造
    Recent_Data(const Recent_Data& other)
        : avatarPath(other.avatarPath),
        msg(other.msg),
        user_id(other.user_id),
        userName(other.userName),
        msg_time(other.msg_time),
        timestamp(other.timestamp),
        UnreadCount(other.UnreadCount)
    {}


    // 带参构造，参数有默认值，方便只填部分字段
    Recent_Data(const QString& avatarPath_,
                const QString& msg_ = {},
                const QString& user_id_ = {},
                const QString& userName_ = {},
                const QDateTime& msg_time_ = {},
                const qint64 & timestamp_ ={},
                const int& unreadCount_ = {})   // 加上未读参数
        : avatarPath(avatarPath_),
        msg(msg_),
        user_id(user_id_),
        userName(userName_),
        msg_time(msg_time_),
        timestamp(timestamp_),
        UnreadCount(unreadCount_) {}

};

enum Recent_Roles{
    avatar_Role = Qt::UserRole+1,
    msg_Role,
    user_id_Role,
    user_name_Role,
    msg_time_Role,
    timestamp_Role,
    unread_count_Role,
    all_data_Role
};





Q_DECLARE_METATYPE(Recent_Data)


#endif // RECENT_DATA_H


