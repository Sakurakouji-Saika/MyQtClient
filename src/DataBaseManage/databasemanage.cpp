#include "databasemanage.h"

#include <QDir>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDateTime>
#include <QDebug>
#include <QMutexLocker>

DataBaseManage *DataBaseManage::instance()
{
    static DataBaseManage inst; // 在第一次调用时创建，程序退出时自动析构
    return &inst;
}

bool DataBaseManage::init(const QString &accountId, const QString &basePath)
{

    QMutexLocker locker(&m_mutex);

    if(accountId.isEmpty() || basePath.isEmpty()){
        qDebug() << "DataBaseManage::init 参数无效";
        return false;
    }

    // 如果已初始化且相同 account，直接返回 true
    if (!m_accountId.isEmpty() && m_accountId == accountId && m_db.isValid() && m_db.isOpen()) {
        qDebug() << "DataBaseManage::init 已初始化过";
        return true;
    }

    // 果已有打开的 db（不同账号），先 close. #感觉可以删除这一段
    if(m_db.isValid() && m_db.isOpen()){
        qDebug() << "DataBaseManage::init 已打开过其他账号";
        close();
    }

    m_accountId = accountId;

    QDir baseDir(basePath);

    qDebug() << "DataBaseManage::init::[basePath]:" << basePath;

    if(!baseDir.exists()){
        if(!QDir().mkpath(basePath)){
            qDebug() << "DataBaseManage::init 无法创建基础路径：" << basePath;
            return false;
        }
    }

    // 创建 account 目录
    QString accountDirPath = QDir(basePath).filePath(accountId);


    QDir accountDir(accountDirPath);
    if(!accountDir.exists()){
        if(!QDir().mkpath(accountDirPath)){
            qDebug() << "DataBaseManage::init 无法创建账户目录：" << accountDirPath;
            return false;
        }
    }

    // 数据库文件命名： accountID.db
    m_dbPath = accountDir.filePath(accountId + ".db");

    // QSqlDatabase 连接名需要唯一
    m_connName = QString("ChatDB_%1").arg(accountId);

    // 打开数据库
    if(!openDatabase()){
        qDebug() << "DataBaseManage::init 打开数据库失败" << accountDirPath;
        return false;
    }

    if(!createTables()){
        close();
        qDebug() << "DataBaseManage::init 创建数据库表失败" << accountDirPath;

        return false;
    }

    // 推荐设置（WAL 模式等），在打开后执行
    {
        QSqlQuery q(m_db);
        q.exec("PRAGMA journal_mode = WAL;");
        q.exec("PRAGMA synchronous = NORMAL;");
        // 可根据需要设置 cache_size、temp_store 等
    }

    qDebug() << "DataBaseMgr::init 成功, dbPath =" << m_dbPath;
    return true;


}

void DataBaseManage::close()
{
    QMutexLocker locker(&m_mutex);

    if(m_db.isValid() && m_db.isOpen()){
        QString conn = m_db.connectionName();
        m_db.close();
        QSqlDatabase::removeDatabase(conn);
        qDebug() << "DataBaseManage::关闭，移除连接:" << conn;
    }
    m_db = QSqlDatabase();
    m_accountId.clear();
    m_dbPath.clear();
    m_connName.clear();
}

Add_Friend_Type DataBaseManage::checkAndAddFriend(const QString &friendId, const QString &displayName, const QString &avatar, int status, const QString &remark)
{
    if(isFriend(friendId)){
        qDebug()<< "DataBaseManage::checkAndAddFriend::[已经是好友了]:" << friendId;
        return Add_Friend_Type::exist;
    }else{
        qDebug()<< "DataBaseManage::checkAndAddFriend::[不是好友]:" << friendId;
        if(addFriend(friendId, displayName, avatar, status, remark)){
            return Add_Friend_Type::success;
        }else{
            qDebug()<< "DataBaseManage::checkAndAddFriend::[插入失败]:" << friendId;
            return Add_Friend_Type::failure;
        }
    }
}

bool DataBaseManage::isFriend(const QString &friendId) const
{
    if (friendId.isEmpty()) return false;

    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) {
        return false;
    }

    QSqlQuery q(m_db);
    q.prepare("SELECT EXISTS(SELECT 1 FROM friend_info WHERE friend_id = :fid)");
    q.bindValue(":fid",friendId);

    if (!q.exec()) {
        qDebug() << "DataBaseMgr::isFriend 错误:" << q.lastError().text();
        return false;
    }


    if (q.next()) {
        qDebug() << "isFriend[value]:" <<q.value(0).toInt();
        return q.value(0).toInt() != 0; // SQLite 的 EXISTS 返回 0 或 1
    }
    return false;

}

bool DataBaseManage::addFriend(const QString &friendId, const QString &displayName, const QString &avatar, int status, const QString &remark)
{
    if (friendId.isEmpty()) return false;

    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) {
        return false;
    }

    QSqlQuery q(m_db);
    q.prepare(R"(
        INSERT OR REPLACE INTO friend_info (friend_id, display_name, avatar, status, remark, created_at)
        VALUES (:fid, :name, :avatar, :status, :remark, :ctime)
    )");

    q.bindValue(":fid", friendId);
    q.bindValue(":name", displayName);
    q.bindValue(":avatar", avatar);
    q.bindValue(":status", status);
    q.bindValue(":remark", remark);
    q.bindValue(":ctime", QDateTime::currentSecsSinceEpoch()); // 或 currentMSecs... 取决于你统一的时间单位

    if (!q.exec()) {
        qDebug() << "DataBaseMgr::addFriend failed:" << q.lastError().text();
        return false;
    }

    return true;
}

bool DataBaseManage::addChatMessage(const QString &msgId, const QString &fromId, const QString &toId, const QString &content, int type, qint64 timestamp)
{
    if (msgId.isEmpty()) return false;

    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return false;

    QSqlQuery q(m_db);
    q.prepare(R"(
        INSERT OR REPLACE INTO chat_records (msg_id, from_id, to_id, content, type, timestamp)
        VALUES (:mid, :from, :to, :content, :type, :ts)
    )");
    q.bindValue(":mid", msgId);
    q.bindValue(":from", fromId);
    q.bindValue(":to", toId);
    q.bindValue(":content", content);
    q.bindValue(":type", type);
    q.bindValue(":ts", timestamp);

    if (!q.exec()) {
        qDebug() << "DataBaseMgr::addChatMessage failed:" << q.lastError().text();
        return false;
    }
    return true;
}

bool DataBaseManage::upsertRecentMessage(const QString &peerId, const QString &lastMsg, qint64 lastTime, int unreadCount, int direction)
{
    if (peerId.isEmpty()) return false;

    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return false;

    QSqlQuery q(m_db);
    q.prepare(R"(
        INSERT INTO recent_messages (peer_id, last_msg, last_time, unread_count, direction)
        VALUES (:peer, :msg, :time, :unread, :dir)
        ON CONFLICT(peer_id) DO UPDATE SET
            last_msg = excluded.last_msg,
            last_time = excluded.last_time,
            unread_count = excluded.unread_count,
            direction = excluded.direction
    )");
    q.bindValue(":peer", peerId);
    q.bindValue(":msg", lastMsg);
    q.bindValue(":time", lastTime);
    q.bindValue(":unread", unreadCount);
    q.bindValue(":dir", direction);

    if (!q.exec()) {
        qDebug() << "DataBaseMgr::upsertRecentMessage failed:" << q.lastError().text();
        return false;
    }
    return true;
}

bool DataBaseManage::isOpen() const
{
    QMutexLocker locker(&m_mutex);
    return m_db.isValid() && m_db.isOpen();
}

QList<FriendInfo> DataBaseManage::getFriendList() const
{
    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return QList<FriendInfo>();

    QList<FriendInfo> temp;

    QSqlQuery query(m_db);

    if (!query.exec("SELECT id,friend_id, display_name, avatar, status, remark, created_at FROM friend_info;")) {
        qWarning() << "select friend_info failed:" << query.lastError().text();
    } else {
        qDebug() << "---- friend_info ----";
        while (query.next()) {
            qDebug() << "id:" << query.value(0).toString()
            << "friend_id:" << query.value(1).toString()
            << "name:" << query.value(2).toString()
            << "avatar:" << query.value(3).toString()
            << "status:" << query.value(4).toInt()
            << "remark:" << query.value(5).toString()
            << "created_at:" << QDateTime::fromSecsSinceEpoch(query.value(6).toLongLong()).toString();

            temp.append(FriendInfo(query.value(0).toInt(),query.value(1).toString(),query.value(2).toString(),query.value(3).toString(),query.value(4).toInt(),query.value(5).toString(),query.value(6).toLongLong()));
        }
    }

    return temp;
}

QList<RecentMessage> DataBaseManage::getRecentMessageList() const
{


    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return QList<RecentMessage>();


    QList<RecentMessage> temp;
    QSqlQuery query(m_db);


    if (!query.exec("SELECT id, peer_id, last_msg, last_time, unread_count, direction FROM recent_messages")) {
                qWarning() << "select chat_records failed:" << query.lastError().text();
            } else {
                qDebug() << "---- chat_records ----";
                while (query.next()) {
                    qDebug() << "msg_id:" << query.value(0).toString()
                    << "from:" << query.value(1).toString()
                    << "to:" << query.value(2).toString()
                    << "content:" << query.value(3).toString()
                    << "type:" << query.value(4).toInt()
                    << "time:" << QDateTime::fromSecsSinceEpoch(query.value(5).toLongLong()).toString();
                    // temp.append(RecentMessage(-1,))
                }
            }
}

bool DataBaseManage::insertOrUpdateRecentMessage(const QString &peerId, const QString &lastMsg, qint64 lastTime, int unreadCount, int direction)
{
    if (peerId.isEmpty()) return false;

    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return false;


    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO recent_messages (peer_id, last_msg, last_time, unread_count, direction)
        VALUES (:peer_id, :last_msg, :last_time, :unread_count, :direction)
        ON CONFLICT(peer_id) DO UPDATE SET
            last_msg = excluded.last_msg,
            last_time = excluded.last_time,
            unread_count = excluded.unread_count,
            direction = excluded.direction
    )");

    query.bindValue(":peer_id", peerId);
    query.bindValue(":last_msg", lastMsg);
    query.bindValue(":last_time", lastTime);
    query.bindValue(":unread_count", unreadCount);
    query.bindValue(":direction", direction);

    if (!query.exec()) {
        qWarning() << "Failed to insert/update recent_messages:" << query.lastError().text();
        return false;
    }

    return true;
}



DataBaseManage::DataBaseManage(QObject *parent)
    : QObject{parent}
{}

DataBaseManage::~DataBaseManage()
{
    close();
}




bool DataBaseManage::createTables()
{
    if (!m_db.isValid() || !m_db.isOpen()) {
        qDebug() << "DataBaseManage::createTables:: db 未打开";
        return false;
    }

    QSqlQuery q(m_db);
    bool ok;

    // friend_info
    ok = q.exec(R"(
        CREATE TABLE IF NOT EXISTS friend_info (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            friend_id TEXT UNIQUE NOT NULL,
            display_name TEXT,
            avatar TEXT,
            status INTEGER DEFAULT 0,
            remark TEXT,
            created_at INTEGER
        );
    )");
    if (!ok) { qDebug() << "create friend_info failed:" << q.lastError().text(); return false; }

    // chat_records
    ok = q.exec(R"(
        CREATE TABLE IF NOT EXISTS chat_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            msg_id TEXT UNIQUE,
            from_id TEXT,
            to_id TEXT,
            content TEXT,
            type INTEGER,
            timestamp INTEGER,
            status INTEGER DEFAULT 0
        );
    )");
    if (!ok) { qDebug() << "create chat_records failed:" << q.lastError().text(); return false; }

    // recent_messages
    ok = q.exec(R"(
        CREATE TABLE IF NOT EXISTS recent_messages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            peer_id TEXT UNIQUE,
            last_msg TEXT,
            last_time INTEGER,
            unread_count INTEGER DEFAULT 0,
            direction INTEGER DEFAULT 0
        );
    )");
    if (!ok) { qDebug() << "create recent_messages failed:" << q.lastError().text(); return false; }

    // 索引（根据查询习惯增加）
    ok = q.exec("CREATE INDEX IF NOT EXISTS idx_chat_timestamp ON chat_records(timestamp);");
    if (!ok) { qDebug() << "create idx_chat_timestamp failed:" << q.lastError().text(); /*非致命*/ }

    ok = q.exec("CREATE INDEX IF NOT EXISTS idx_chat_from_to ON chat_records(from_id, to_id);");
    if (!ok) { qDebug() << "create idx_chat_from_to failed:" << q.lastError().text(); /*非致命*/ }

    return true;
}



bool DataBaseManage::openDatabase()
{

    // 如果已存在同名连接，先移除（因为 QSqlDatabase::removeDatabase 要求先 close 并移除） #个人感觉其实这里都可以不用的，但主要是为了有个大概印象才写的
    if (QSqlDatabase::contains(m_connName)) {
        {
            QSqlDatabase old = QSqlDatabase::database(m_connName);
            if (old.isOpen()) old.close();
        } // old 在此析构，释放对连接的引用
        QSqlDatabase::removeDatabase(m_connName);
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE",m_connName);
    m_db.setDatabaseName(m_dbPath);

    if(!m_db.open()){
        qDebug() << "DataBaseManage::createTables::[打开数据库失败]:" <<m_db.lastError().text();
        qDebug() << "DataBaseManage::createTables::[打开数据库失败]::[路径]" << m_dbPath;
        return false;
    }

    return true;

}
