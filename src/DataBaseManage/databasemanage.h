#ifndef DATABASEMANAGE_H
#define DATABASEMANAGE_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
#include <QDateTime>
#include "model/FriendInfo.h"
#include "model/recent_messages.h"
#include "model/ChatRecord.h"
#include "model/FriendsResponse.h"

enum Add_Friend_Type{
    failure = 0,    // 失败
    success = 1,    // 成功
    exist = 2       // 存在
};


class FriendAvatar;

class DataBaseManage : public QObject
{
    Q_OBJECT
public:
    // 获取单例
    static DataBaseManage *instance();

    // 初始化：accountId 与 basePath 必须提供
    // 返回 true 表示初始化并打开数据库成功（并创建表）
    bool init(const int &accountId, const QString &basePath);

    // 关闭数据库连接（会移除 QSqlDatabase 连接）
    void close();

    Add_Friend_Type checkAndAddFriend(const int &friendId,const QString &displayName,const QString &avatar = QString(),int status = 0,const QString &remark = QString());

    // 插入聊天消息
    bool addChatMessage(const QString &msgId,const QString &fromId,const QString &toId,const QString &content,int type,qint64 timestamp);

    // 插入或更新最近会话信息（peerId 唯一）
    bool upsertRecentMessage(const QString &peerId,const QString &lastMsg,qint64 lastTime,int unreadCount,int direction);

    // 原子地插入聊天消息并更新最近会话（在同一事务中执行）
    bool addChatMessageAndUpdateRecent(const QString &msgId,
                                       const int &fromId,
                                       const int &toId,
                                       const QString &content,
                                       int type,
                                       qint64 timestamp,
                                       const int &peerId,
                                       const QString &lastMsg,
                                       qint64 lastTime,
                                       int unreadCount,
                                       int direction);

    // 判断是否已初始化并打开
    bool isOpen() const;

    // 获取数据库文件路径（只读）
    QString dbPath() const { return m_dbPath; }

    // 获取当前连接名（调试用）
    QString connectionName() const { return m_connName; }

    // 查询 friend_info 表
    QList<FriendInfo> getFriendList() const;

    // 查询 最近对话记录
    QList<RecentMessage> getRecentMessageList() const;



    // 插入最近对话
    bool insertOrUpdateRecentMessage(
                                const QString &peerId,
                                const QString &lastMsg,
                                qint64 lastTime,
                                int unreadCount = 0,
                                     int direction = 0);

    // 查找头像
    QString getAvatarByFriendId(const qint64 &friendId);

    // 查找用户名
    QString getDisplayNameByFriendId(const qint64  &friendId);

    // 查找聊天记录
    QList<ChatRecord> getChatRecords(const int &userA, const int &userB);

    // 插入一条聊天记录
    bool addChatRecords(const QList<ChatRecord> &records);

    // 插入一条记录
    int getTotalUnreadCount();

    // 判断是不是已经是好友了
    bool isFriend(const int &friendId) const;

    // 判断是不是已经是好友了(字符串)
    bool isFriend(const QString &friendid) const;

    // 基本写接口（返回是否成功）
    // bool addFriend(const int &friendId,const QString &displayName,const QString &avatar = QString(),int status = 0,const QString &remark = QString());

    // 插入单个好友
    bool upsertFriend(qint64 friendId,
                      const QString &username,
                      const QString &nickname,
                      const QString &email,
                      const QString &avatar_file_id,
                      const QString &avatar,
                      const int &status,
                      qint64 createdAt = 0,
                      qint64 updatedAt = 0);

    // 初始化获取好友列表
    bool saveFriendListToDb(const FriendsResponse &resp);

    // 获取用户头像数据，（通过文件ID来查找）
    std::optional<FriendInfo> GetFriendAvatarById(qint64 uid);


    // 更新用户头像数据
    bool UpdateFriendAvatarByAvatarID(const qint64 avatar_file_id, const QString avatar);

    // 更新用户头像完成,将新的文件id 和 文件名 写入到用户表.
    bool updateUserAvatarById(qint64 userId, qint64 avatarFileId, const QString &avatarFileName);

    // 获取用户头像数据
    void GetUserAvatarData(QList<FriendAvatar> &result);


private:
    explicit DataBaseManage(QObject *parent = nullptr);
    ~DataBaseManage() override;

    DataBaseManage(const DataBaseManage&) = delete;
    DataBaseManage &operater(const DataBaseManage&) = delete;


    // 打开数据库（内部调用）
    bool openDatabase();

    // 创建表（内部调用）
    bool createTables();




private:
    // inline static DataBaseManage *m_instance;

    QString m_accountId;
    QString m_dbPath;
    QString m_connName;
    QSqlDatabase m_db;
    mutable QMutex m_mutex;
};

#endif // DATABASEMANAGE_H
