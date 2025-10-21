#ifndef DATABASEMANAGE_H
#define DATABASEMANAGE_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>

class DataBaseManage : public QObject
{
    Q_OBJECT
public:
    // 获取单例
    static DataBaseManage *instance();

    // 初始化：accountId 与 basePath 必须提供
    // 返回 true 表示初始化并打开数据库成功（并创建表）
    bool init(const QString &accountId, const QString &basePath);

    // 关闭数据库连接（会移除 QSqlDatabase 连接）
    void close();

    // 判断是不是已经是好友了
    bool isFriend(const QString &friendId) const;

    // 基本写接口（返回是否成功）
    bool addFriend(const QString &friendId,const QString &displayName,const QString &avatar = QString(),int status = 0,const QString &remark = QString());

    // 插入聊天消息
    bool addChatMessage(const QString &msgId,const QString &fromId,const QString &toId,const QString &content,int type,qint64 timestamp);

    // 插入或更新最近会话信息（peerId 唯一）
    bool upsertRecentMessage(const QString &peerId,const QString &lastMsg,qint64 lastTime,int unreadCount,int direction);

    // 判断是否已初始化并打开
    bool isOpen() const;

    // 获取数据库文件路径（只读）
    QString dbPath() const { return m_dbPath; }

    // 获取当前连接名（调试用）
    QString connectionName() const { return m_connName; }

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
