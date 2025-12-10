#ifndef FRIENDINFO_H
#define FRIENDINFO_H


#include <QString>
#include <QDateTime>

struct FriendInfo
{
    int id;                // 本地自增主键
    qint64 friendId;       // 服务端用户 id（64 位）
    QString username;      // 登录名（服务端字段）
    QString nickname;      // 客户端/用户设置的昵称（优先显示）
    QString email;         // 邮箱
    QString avatarFileId;  // 服务端头像文件 id
    QString avatar;        // 客户端使用的头像 URL 或本地缓存路径
    bool status;        // 好友状态，例如 "1" / "0"
    qint64 createdAt;      // 本地记录创建时间（UNIX 秒）
    qint64 updatedAt;      // 服务端最后更新时间（UNIX 秒）

    // 默认构造函数：id=-1 表示未持久化，status 默认为 "offline",
    // createdAt 默认为当前时间戳，updatedAt 可为 0 表示未知
    FriendInfo()
        : id(-1),
        friendId(0),
        username(),
        nickname(),
        email(),
        avatarFileId(),
        avatar(),
        status(true),
        createdAt(QDateTime::currentSecsSinceEpoch()),
        updatedAt(0)
    {}

    // 全参构造
    FriendInfo(int t_id,
               qint64 t_friendId,
               const QString &t_username,
               const QString &t_nickname,
               const QString &t_email,
               const QString &t_avatarFileId,
               const QString &t_avatar,
               const bool &t_status,
               qint64 t_createdAt,
               qint64 t_updatedAt)
        : id(t_id),
        friendId(t_friendId),
        username(t_username),
        nickname(t_nickname),
        email(t_email),
        avatarFileId(t_avatarFileId),
        avatar(t_avatar),
        status(t_status),
        createdAt(t_createdAt),
        updatedAt(t_updatedAt)
    {}

    // 返回用于 UI 显示的名称：优先使用 nickname，否则返回 username
    QString displayName() const
    {
        return !nickname.isEmpty() ? nickname : username;
    }

    // 是否在线（简单判断，取决于 status 字段的具体定义）
    bool isOnline() const
    {
        return status;
    }
};



#endif // FRIENDINFO_H
