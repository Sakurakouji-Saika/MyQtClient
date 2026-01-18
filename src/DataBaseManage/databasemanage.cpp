#include "databasemanage.h"

#include <QDir>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDateTime>
#include <QDebug>
#include <QMutexLocker>
#include "model/FriendsResponse.h"
#include "ViewModel/FriendAvatarDTO.h"

DataBaseManage *DataBaseManage::instance()
{

    static DataBaseManage inst; // 在第一次调用时创建，程序退出时自动析构
    return &inst;
}

bool DataBaseManage::init(const int &accountId, const QString &basePath)
{

    QMutexLocker locker(&m_mutex);



    // 如果已初始化且相同 account，直接返回 true
    if (!m_accountId.isEmpty() && m_accountId.toInt() == accountId && m_db.isValid() && m_db.isOpen()) {
        qDebug() << "DataBaseManage::init 已初始化过";
        return true;
    }

    // 果已有打开的 db（不同账号），先 close. #感觉可以删除这一段
    if(m_db.isValid() && m_db.isOpen()){
        qDebug() << "DataBaseManage::init 已打开过其他账号";
        close();
    }

    m_accountId = QString::number(accountId);

    QDir baseDir(basePath);

    qDebug() << "DataBaseManage::init::[basePath]:" << basePath;

    if(!baseDir.exists()){
        if(!QDir().mkpath(basePath)){
            qDebug() << "DataBaseManage::init 无法创建基础路径：" << basePath;
            return false;
        }
    }

    // 创建 account 目录
    QString accountDirPath = QDir(basePath).filePath(QString::number(accountId));


    QDir accountDir(accountDirPath);
    if(!accountDir.exists()){
        if(!QDir().mkpath(accountDirPath)){
            qDebug() << "DataBaseManage::init 无法创建账户目录：" << accountDirPath;
            return false;
        }
    }

    // 数据库文件命名： accountID.db
    m_dbPath = accountDir.filePath(QString::number(accountId) + ".db");

    // QSqlDatabase 连接名需要唯一
    m_connName = QString("ChatDB_%1").arg(QString::number(accountId));

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

Add_Friend_Type DataBaseManage::checkAndAddFriend(const int &friendId, const QString &displayName, const QString &avatar, int status, const QString &remark)
{
    // 如果已经是好友
    if (isFriend(friendId)) {
        qDebug() << "DataBaseManage::checkAndAddFriend::[已经是好友了]:" << friendId;
        return Add_Friend_Type::exist;
    }

    qDebug() << "DataBaseManage::checkAndAddFriend::[不是好友]:" << friendId;



    // 把 displayName 写入 nickname，remark 如果需要可写进另一个字段或存到 nickname 的括号里
    QString nickname = displayName;
    if (!remark.isEmpty()) {
        // 可选：把 remark 合并进 nickname（或单独保存），这里示例不合并，仅注释说明
        // nickname += " (" + remark + ")";
    }

    // avatar 参数 直接写入 avatar 字段；avatar_file_id 暂时留空
    bool ok = upsertFriend(friendId,
                           /*username*/ QString(),
                           /*nickname*/ nickname,
                           /*email*/ QString(),
                           /*avatar_file_id*/ qint64(),
                           /*avatar*/ avatar,
                           /*status*/ status,
                           /*createdAt*/ QDateTime::currentSecsSinceEpoch(),
                           /*updatedAt*/ 0);

    if (ok) {
        return Add_Friend_Type::success;
    } else {
        qDebug() << "DataBaseManage::checkAndAddFriend::[插入失败]:" << friendId;
        return Add_Friend_Type::failure;
    }
}

bool DataBaseManage::isFriend(const int &friendId) const
{

    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) {
        return false;
    }

    QSqlQuery q(m_db);
    q.prepare("SELECT EXISTS(SELECT 1 FROM friend_info WHERE friend_id = :fid)");
    q.bindValue(":fid", QVariant::fromValue(friendId));

    if (!q.exec()) {
        qDebug() << "DataBaseMgr::isFriend 错误:" << q.lastError().text();
        return false;
    }

    if (q.next()) {
        return q.value(0).toInt() != 0;
    }
    return false;

}

bool DataBaseManage::isFriend(const QString &friendId) const
{
    if(friendId.isEmpty()) return false;

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

bool DataBaseManage::upsertFriend(qint64 friendId, const QString &username, const QString &nickname, const QString &email, const qint64 &avatar_file_id, const QString &avatar, const int &status, qint64 createdAt, qint64 updatedAt)
{
    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return false;

    QSqlQuery q(m_db);
    q.prepare(R"(
        INSERT INTO friend_info
            (friend_id, username, nickname, email, avatar_file_id, avatar, status, created_at, updated_at)
        VALUES
            (:friend_id, :username, :nickname, :email, :avatar_file_id, :avatar, :status, :created_at, :updated_at)
        ON CONFLICT(friend_id) DO UPDATE SET
            username = excluded.username,
            nickname = excluded.nickname,
            email = excluded.email,
            avatar_file_id = excluded.avatar_file_id,
            avatar = excluded.avatar,
            status = excluded.status,
            created_at = excluded.created_at,
            updated_at = excluded.updated_at
    )");

    q.bindValue(":friend_id", QVariant::fromValue(friendId));
    q.bindValue(":username", username);
    q.bindValue(":nickname", nickname);
    q.bindValue(":email", email);
    q.bindValue(":avatar_file_id", avatar_file_id);
    q.bindValue(":avatar", avatar);

    q.bindValue(":status", status);



    q.bindValue(":created_at", createdAt ? createdAt : QDateTime::currentSecsSinceEpoch());
    q.bindValue(":updated_at", updatedAt);

    qDebug() << "-----------11111111111---------";

    if (!q.exec()) {

        qDebug() << "DataBaseMgr::upsertFriend failed:" << q.lastError().text();
        return false;
    }
    return true;
}

bool DataBaseManage::saveFriendListToDb(const FriendsResponse &resp)
{
    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return false;

    const FriendsResponse &r = resp;
    int myUserId = r.userId;

    // 开事务
    if (!m_db.transaction()) {
        qWarning() << "DB transaction start failed:" << m_db.lastError().text();
        return false;
    }

    // 预编译语句（使用 UPDATE -> INSERT 的可移植 upsert）
    QSqlQuery qUpdateFriend(m_db);
    qUpdateFriend.prepare(
        "UPDATE friend_info SET username = :username, nickname = :nickname, "
        "avatar = :avatar, updated_at = :updated_at, avatar_file_id = :avatar_file_id, "
        "status = :status "
        "WHERE friend_id = :friend_id;"
        );

    QSqlQuery qInsertFriend(m_db);
    qInsertFriend.prepare(
        "INSERT INTO friend_info (friend_id, username, nickname, avatar, created_at, updated_at, avatar_file_id, status) "
        "VALUES (:friend_id, :username, :nickname, :avatar, :created_at, :updated_at, :avatar_file_id, :status);"
        );

    QSqlQuery qUpdateRecent(m_db);
    qUpdateRecent.prepare(
        "UPDATE recent_messages SET last_msg = :last_msg, last_time = :last_time, unread_count = :unread_count, direction = :direction "
        "WHERE peer_id = :peer_id;"
        );

    QSqlQuery qInsertRecent(m_db);
    qInsertRecent.prepare(
        "INSERT INTO recent_messages (peer_id, last_msg, last_time, unread_count, direction) "
        "VALUES (:peer_id, :last_msg, :last_time, :unread_count, :direction);"
        );

    QSqlQuery qInsertChat(m_db);
    qInsertChat.prepare(
        "INSERT OR IGNORE INTO chat_records (msg_id, from_id, to_id, content, type, timestamp, status) "
        "VALUES (:msg_id, :from_id, :to_id, :content, :type, :timestamp, :status);"
        );


    qDebug()<< "DataBaseManage::saveFriendListToDb::好友列表数量::" <<r.friends.size();

    // 遍历好友数组
    for (const FriendInfo_sever &fi : r.friends) {


        int friendId = fi.id;
        QString username = fi.username;
        QString nickname = fi.nickname;
        QString avatar = fi.avatarPath;
        int unread = fi.unreadCount;
        qint64 avatar_file_id = fi.avatar_file_id;
        qint64 nowSec = QDateTime::currentSecsSinceEpoch();
        bool status = fi.state;

        qDebug() << "DataBaseManage::saveFriendListToDb::好友状态:UID" << friendId << " || NAME:"  << username << status;



        // 1) friend_info: update first
        qUpdateFriend.bindValue(":username", username);
        qUpdateFriend.bindValue(":nickname", nickname);

        qUpdateFriend.bindValue(":avatar", avatar);
        qUpdateFriend.bindValue(":updated_at", nowSec);
        qUpdateFriend.bindValue(":avatar_file_id", avatar_file_id);

        qUpdateFriend.bindValue(":friend_id", friendId);
        qUpdateFriend.bindValue(":status", status);


        if (!qUpdateFriend.exec()) {
            qWarning() << "UPDATE friend_info failed for" << friendId << ":" << qUpdateFriend.lastError().text();
            m_db.rollback();
            return false;
        }

        // 如果没有更新任何行，执行 INSERT
        if (qUpdateFriend.numRowsAffected() == 0) {
            qInsertFriend.bindValue(":friend_id", friendId);
            qInsertFriend.bindValue(":username", username);
            qInsertFriend.bindValue(":nickname", nickname);

            qInsertFriend.bindValue(":avatar", avatar);
            qInsertFriend.bindValue(":created_at", nowSec);
            qUpdateFriend.bindValue(":avatar_file_id", avatar_file_id);

            qInsertFriend.bindValue(":updated_at", nowSec);

            qInsertFriend.bindValue(":status", status);


            if (!qInsertFriend.exec()) {
                qWarning() << "INSERT friend_info failed for" << friendId << ":" << qInsertFriend.lastError().text();
                m_db.rollback();
                return false;
            }
        }

        // 2) 解析 last message（可能为 null）
        QString lastMsgStr;
        qint64 lastMsgTime = 0;
        int direction = 0; // 0 = 对方发来, 1 = 我发出

        if (fi.lastMessage.has_value()) {
            const LastMessageInfo &lm = *fi.lastMessage;
            lastMsgStr = lm.content;
            if (lm.sentAt.isValid()) lastMsgTime = lm.sentAt.toSecsSinceEpoch();
            else lastMsgTime = 0;
            direction = (lm.senderId == myUserId) ? 1 : 0;

            // 3) 尝试插入 chat_records（忽略重复 msg_id）
            QString msgIdStr = QString::number(lm.id);
            QString fromIdStr = QString::number(lm.senderId);
            QString toIdStr = QString::number((lm.senderId == myUserId) ? friendId : myUserId);



            qInsertChat.bindValue(":msg_id", msgIdStr);
            qInsertChat.bindValue(":from_id", fromIdStr);
            qInsertChat.bindValue(":to_id", toIdStr);
            qInsertChat.bindValue(":content", lm.content);
            qInsertChat.bindValue(":type", messageTypeToInt(lm.type));
            qInsertChat.bindValue(":timestamp", lastMsgTime);
            qInsertChat.bindValue(":status", status);


            if (!qInsertChat.exec()) {
                qWarning() << "INSERT chat_records failed for msg" << msgIdStr << ":" << qInsertChat.lastError().text();
                m_db.rollback();
                return false;
            }
        } else {
            // 没有 last message
            lastMsgStr = "";
            lastMsgTime = 0;
            direction = 0;
        }

        // 4) recent_messages: update then insert
        QString peerIdStr = QString::number(friendId);
        qUpdateRecent.bindValue(":last_msg", lastMsgStr);
        qUpdateRecent.bindValue(":last_time", lastMsgTime);
        qUpdateRecent.bindValue(":unread_count", unread);
        qUpdateRecent.bindValue(":direction", direction);
        qUpdateRecent.bindValue(":peer_id", peerIdStr);

        if (!qUpdateRecent.exec()) {
            qWarning() << "UPDATE recent_messages failed for peer" << peerIdStr << ":" << qUpdateRecent.lastError().text();
            m_db.rollback();
            return false;
        }

        if (qUpdateRecent.numRowsAffected() == 0) {
            qInsertRecent.bindValue(":peer_id", peerIdStr);
            qInsertRecent.bindValue(":last_msg", lastMsgStr);
            qInsertRecent.bindValue(":last_time", lastMsgTime);
            qInsertRecent.bindValue(":unread_count", unread);
            qInsertRecent.bindValue(":direction", direction);
            if (!qInsertRecent.exec()) {
                qWarning() << "INSERT recent_messages failed for peer" << peerIdStr << ":" << qInsertRecent.lastError().text();
                m_db.rollback();
                return false;
            }
        }

    }

    // 提交事务
    if (!m_db.commit()) {
        qWarning() << "DB commit failed:" << m_db.lastError().text();
        m_db.rollback();
        return false;
    }

    return true;
}

std::optional<FriendInfo> DataBaseManage::GetFriendAvatarById(qint64 uid)
{
    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) {
        return std::nullopt;
    }

    QSqlQuery q(m_db);
    q.prepare(R"(
        SELECT id, avatar_file_id, avatar
        FROM friend_info
        WHERE friend_id = :uid
        LIMIT 1;
    )");
    q.bindValue(":uid", uid);

    if (!q.exec()) {
        qDebug() << "GetFriendAvatarById failed:" << q.lastError().text();
        return std::nullopt;
    }

    if (!q.next()) {
        // 没查到记录
        return std::nullopt;
    }

    FriendInfo info; // 使用默认构造，其余字段保持默认值

    info.id = q.value("id").toInt();
    info.avatarFileId = q.value("avatar_file_id").toString();
    info.avatar = q.value("avatar").toString();


    return info;
}

bool DataBaseManage::UpdateFriendAvatarByAvatarID(const qint64 avatar_file_id, const QString avatar)
{
    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return false;

    QSqlQuery q(m_db);
    q.prepare(R"(
        UPDATE friend_info SET avatar = :avatar WHERE avatar_file_id = :avatar_file_id
    )");

    q.bindValue(":avatar_file_id", avatar_file_id);
    q.bindValue(":avatar", avatar);

    if (!q.exec()) {
        qDebug() << "DataBaseMgr::UpdateFriendAvatarByID failed:" << q.lastError().text();
        return false;
    }
    return true;
}

bool DataBaseManage::updateUserAvatarById(qint64 userId, qint64 avatarFileId, const QString &avatarFileName)
{
    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return false;

    QSqlQuery q(m_db);
    q.prepare(R"(
        UPDATE friend_info
        SET avatar_file_id=:avatar_file_id , avatar=:avatar
        WHERE id = :uid;
    )");

    q.bindValue(":avatar_file_id", QString::number(avatarFileId));
    q.bindValue(":avatar", QString(avatarFileName));
    q.bindValue(":uid", userId);

    if (!q.exec()) {
        qDebug() << "DataBaseManage::updateUserAvatarById failed:" << q.lastError().text();
        return false;
    } else {
        qDebug() << "update ok, rowsAffected =" << q.numRowsAffected();
    }

    return true;
}

void DataBaseManage::GetUserAvatarData(QList<FriendAvatar> &result)
{
    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return;

    QSqlQuery q(m_db);
    q.prepare(R"(
        SELECT friend_id, avatar
        FROM friend_info;
    )");

    if (!q.exec()) {
        qDebug() << "DataBaseMgr::GetUserAvatarData exec failed:" << q.lastError().text();
        return;
    }

    QSqlRecord rec = q.record();
    int idxFriendId = rec.indexOf("friend_id");
    int idxAvatar = rec.indexOf("avatar");
    if (idxFriendId < 0 || idxAvatar < 0) {
        qDebug() << "Expected columns not found in record";
        return;
    }

    while (q.next()) {
        FriendAvatar t;
        t.uid = q.value(idxFriendId).toLongLong();
        t.fileName = q.value(idxAvatar).toString();
        result.append(t);
    }
}

bool DataBaseManage::deleteFriendByUID(qint64 friend_uid)
{
    if (friend_uid <= 0) return false;

    QMutexLocker locker(&m_mutex);

    if (!m_db.isValid() || !m_db.isOpen()) return false;

    if (!m_db.transaction()) {
        qDebug() << "DB transaction start failed:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery q(m_db);

    q.prepare("DELETE FROM friend_info WHERE friend_id = :friend_id");
    q.bindValue(":friend_id", friend_uid);

    if (!q.exec()) {
        qDebug() << "删除好友失败:" << q.lastError().text();
        m_db.rollback();
        return false;
    }

    q.clear();
    q.prepare("DELETE FROM recent_messages WHERE peer_id = :friend_uid");
    q.bindValue(":friend_uid", QString::number(friend_uid));  // 关键修复

    if (!q.exec()) {
        qDebug() << "删除最近消息失败:" << q.lastError().text();
        m_db.rollback();
        return false;
    }

    if (!m_db.commit()) {
        qDebug() << "提交事务失败:" << m_db.lastError().text();
        m_db.rollback();
        return false;
    }

    return true;
}

bool DataBaseManage::updateFriendStateByUid(qint64 friend_uid, int state)
{
    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return false;

    QSqlQuery q(m_db);
    q.prepare(R"(
        UPDATE friend_info
        SET status = :state
        WHERE friend_id = :friend_id;
    )");

    q.bindValue(":state", state);
    q.bindValue(":friend_id", friend_uid);


    if (!q.exec()) {
        qDebug() << "DataBaseManage::updateFriendStateByUid failed:" << q.lastError().text();
        return false;
    } else {
        qDebug() << "updateFriendStateByUid ok, rowsAffected =" << q.numRowsAffected();
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
    QList<FriendInfo> temp;
    if (!m_db.isValid() || !m_db.isOpen()) return temp;

    QSqlQuery query(m_db);
    if (!query.exec("SELECT id, friend_id, username, nickname, email, avatar_file_id, avatar, status, created_at, updated_at FROM friend_info;")) {
        qWarning() << "select friend_info failed:" << query.lastError().text();
        return temp;
    }

    // qDebug() << "---- friend_info ----";
    while (query.next()) {
        int id = query.value(0).toInt();
        qint64 friend_id = query.value(1).toLongLong();
        QString username = query.value(2).toString();
        QString nickname = query.value(3).toString();
        QString email = query.value(4).toString();
        QString avatar_file_id = query.value(5).toString();
        QString avatar = query.value(6).toString();
        bool status = query.value(7).toBool();
        qint64 created_at = query.value(8).toLongLong();
        qint64 updated_at = query.value(9).toLongLong();


        FriendInfo fi;
        fi.id = id;
        fi.friendId = friend_id;
        fi.username = username;
        fi.nickname = nickname;
        fi.email = email;
        fi.avatarFileId = avatar_file_id;
        fi.avatar = avatar;
        fi.status = status;
        fi.createdAt = created_at;
        fi.updatedAt = updated_at;

        temp.append(fi);
    }

    return temp;
}

QList<RecentMessage> DataBaseManage::getRecentMessageList() const
{
    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return QList<RecentMessage>();

    QList<RecentMessage> temp;
    QSqlQuery query(m_db);

    const QString sql = QStringLiteral(
        "SELECT id, peer_id, last_msg, last_time, unread_count, direction "
        "FROM recent_messages"
        );

    if (!query.exec(sql)) {
        qWarning() << "select recent_messages failed:" << query.lastError().text();
        return temp;
    }


    while (query.next()) {
        // 列：0=id, 1=peer_id, 2=last_msg, 3=last_time, 4=unread_count, 5=direction
        RecentMessage r;
        r.id = query.value(0).toInt();
        r.peer_id = query.value(1).toInt();


        r.last_msg = query.value(2).toString();

        // 处理时间戳：自动判断是秒级还是毫秒级
        qint64 ts = query.value(3).toLongLong();
        QDateTime dt = (ts > 1000000000000LL)
                           ? QDateTime::fromMSecsSinceEpoch(ts)
                           : QDateTime::fromSecsSinceEpoch(ts);
        r.last_time = ts; // 保持原始时间戳（如果 RecentMessage 存为 qint64）
        // 如果 RecentMessage 存储为 QDateTime，可以改为： r.last_time_dt = dt;

        r.unread_count = query.value(4).toInt();
        r.direction = query.value(5).toInt();

        // // Debug 输出（可选）
        // qDebug() << "id:" << r.id
        //          << "peer_id:" << r.peer_id
        //          << "last_msg:" << r.last_msg
        //          << "last_time(raw):" << ts
        //          << "last_time(str):" << dt.toString("yyyy-MM-dd HH:mm:ss")
        //          << "unread_count:" << r.unread_count
        //          << "direction:" << r.direction;

        temp.append(r);
    }

    return temp;
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

bool DataBaseManage::addChatMessageAndUpdateRecent(const QString &msgId,
                                                   const int &fromId,
                                                   const int &toId,
                                                   const QString &content,
                                                   int type,
                                                   qint64 timestamp,
                                                   const int &peerId,
                                                   const QString &lastMsg,
                                                   qint64 lastTime,
                                                   int unreadCount,
                                                   int direction)
{

    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return false;

    // 使用事务保证两条写入的一致性
    if (!m_db.transaction()) {
        qWarning() << "addChatMessageAndUpdateRecent: failed to start transaction:" << m_db.lastError().text();
        return false;
    }

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
        qWarning() << "addChatMessageAndUpdateRecent: insert chat_records failed:" << q.lastError().text();
        m_db.rollback();
        return false;
    }

    // upsert recent_messages
    QSqlQuery q2(m_db);
    q2.prepare(R"(
        INSERT INTO recent_messages (peer_id, last_msg, last_time, unread_count, direction)
        VALUES (:peer, :msg, :time, :unread, :dir)
        ON CONFLICT(peer_id) DO UPDATE SET
            last_msg = excluded.last_msg,
            last_time = excluded.last_time,
            unread_count = excluded.unread_count,
            direction = excluded.direction
    )");
    q2.bindValue(":peer", peerId);
    q2.bindValue(":msg", lastMsg);
    q2.bindValue(":time", lastTime);
    q2.bindValue(":unread", unreadCount);
    q2.bindValue(":dir", direction);

    if (!q2.exec()) {
        qWarning() << "addChatMessageAndUpdateRecent: upsert recent_messages failed:" << q2.lastError().text();
        m_db.rollback();
        return false;
    }

    if (!m_db.commit()) {
        qWarning() << "addChatMessageAndUpdateRecent: commit failed:" << m_db.lastError().text();
        m_db.rollback();
        return false;
    }

    return true;
}

QString DataBaseManage::getAvatarByFriendId(const qint64 &friendId)
{
    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return QString();

    QSqlQuery query(m_db);
    query.prepare("SELECT avatar_file_id FROM friend_info WHERE friend_id = :friend_id");
    query.bindValue(":friend_id", QVariant::fromValue(friendId));

    if (!query.exec()) {
        qWarning() << "Failed to get avatar:" << query.lastError().text();
        return QString();
    }
    if (query.next()) return query.value(0).toString();
    return QString();
}


QString DataBaseManage::getDisplayNameByFriendId(const qint64 &friendId)
{

    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return QString();

    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT
            CASE
                WHEN nickname IS NOT NULL AND nickname <> '' THEN nickname
                ELSE username
            END AS name
        FROM friend_info
        WHERE friend_id = :friend_id
    )");
    query.bindValue(":friend_id", QVariant::fromValue(friendId));

    if (!query.exec()) {
        qWarning() << "Failed to get display name:" << query.lastError().text();
        return QString();
    }
    if (query.next()) return query.value("name").toString();
    return QString();

}

QList<ChatRecord> DataBaseManage::getChatRecords(const int &userA, const int &userB)
{
    QList<ChatRecord> records;
    QMutexLocker locker(&m_mutex);

    if (!m_db.isValid() || !m_db.isOpen()) {
        qWarning() << "Database is not open or invalid!";
        return records;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT *
        FROM chat_records
        WHERE (from_id = :userA AND to_id = :userB)
           OR (from_id = :userB AND to_id = :userA)
        ORDER BY timestamp ASC
    )");
    query.bindValue(":userA", userA);
    query.bindValue(":userB", userB);

    if (!query.exec()) {
        qWarning() << "getChatRecords failed:" << query.lastError().text();
        return records;
    }

    while (query.next()) {
        ChatRecord rec;
        rec.id = query.value("id").toInt();
        rec.msgId = query.value("msg_id").toString();
        rec.fromId = query.value("from_id").toInt();
        rec.toId = query.value("to_id").toInt();
        rec.content = query.value("content").toString();
        rec.type = query.value("type").toInt();
        rec.timestamp = query.value("timestamp").toLongLong();
        rec.status = query.value("status").toInt();
        records.append(rec);
    }

    return records;
}

bool DataBaseManage::addChatRecords(const QList<ChatRecord> &records)
{
    if (records.isEmpty()) return true;

    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return false;

    QSqlQuery q(m_db);
    if (!m_db.transaction()) {
        qWarning() << "Failed to start transaction:" << m_db.lastError().text();
        return false;
    }

    q.prepare(R"(
        INSERT OR REPLACE INTO chat_records
        (msg_id, from_id, to_id, content, type, timestamp, status)
        VALUES (:mid, :from, :to, :content, :type, :ts, :status)
    )");

    for (const ChatRecord &rec : records) {
        if (rec.msgId.isEmpty()) continue;  // 跳过无效记录

        q.bindValue(":mid", rec.msgId);
        q.bindValue(":from", rec.fromId);
        q.bindValue(":to", rec.toId);
        q.bindValue(":content", rec.content);
        q.bindValue(":type", rec.type);
        q.bindValue(":ts", rec.timestamp);
        q.bindValue(":status", rec.status);

        if (!q.exec()) {
            qWarning() << "Insert failed:" << q.lastError().text();
            m_db.rollback();
            return false;
        }
    }

    if (!m_db.commit()) {
        qWarning() << "Commit failed:" << m_db.lastError().text();
        return false;
    }

    return true;
}

int DataBaseManage::getTotalUnreadCount()
{
    QMutexLocker locker(&m_mutex);
    if (!m_db.isValid() || !m_db.isOpen()) return false;

    QSqlQuery query(m_db);

    if (!m_db.transaction()) {
        qWarning() << "Failed to start transaction:" << m_db.lastError().text();
        return false;
    }

    if (!query.exec("SELECT SUM(unread_count) AS total_unread FROM recent_messages;")) {
        qDebug() << "查询失败:" << query.lastError().text();
        return 0;
    }

    int totalUnread = 0;
    // 读取结果
    if (query.next()) {
        totalUnread = query.value("total_unread").toInt();  // 或 query.value(0).toInt()
    }

    return totalUnread;
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

    // friend_info (friend_id 使用长整型)
    ok = q.exec(R"(
        CREATE TABLE IF NOT EXISTS friend_info (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            friend_id INTEGER UNIQUE NOT NULL,         -- 服务端用户 id（64-bit 整数）
            username TEXT NOT NULL,                    -- 服务端 username（登录名）
            nickname TEXT,                             -- 客户端/用户设置的昵称（优先显示）
            email TEXT,                                -- 邮箱
            avatar_file_id TEXT,                       -- 服务端的头像文件 id（字符串）
            avatar TEXT,                               -- 客户端使用的头像 URL 或本地缓存路径
            status INTEGER DEFAULT 0,                  -- 好友状态：0=false, 1=true
            created_at INTEGER DEFAULT (strftime('%s','now')), -- 本地记录创建时间（UNIX 秒）
            updated_at INTEGER                         -- 服务端最后更新时间（UNIX 秒）
        );
    )");
    if (!ok) { qDebug() << "create friend_info failed:" << q.lastError().text(); return false; }

    ok = q.exec("CREATE INDEX IF NOT EXISTS idx_friend_info_friend_id ON friend_info(friend_id);");
    if (!ok) { qDebug() << "create idx_friend_info_friend_id failed:" << q.lastError().text(); /*非致命*/ }

    ok = q.exec("CREATE INDEX IF NOT EXISTS idx_friend_info_username ON friend_info(username);");
    if (!ok) { qDebug() << "create idx_friend_info_username failed:" << q.lastError().text(); /*非致命*/ }


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
