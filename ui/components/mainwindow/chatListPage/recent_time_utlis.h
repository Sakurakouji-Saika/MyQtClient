#ifndef RECENT_TIME_UTLIS_H
#define RECENT_TIME_UTLIS_H

#include <QDateTime>


// 输入：UTC 毫秒时间戳（ms）
// 输出：根据规则返回对应的字符串
inline QString formatMessageTimeSmart2(qint64 ms) {
    if (ms <= 0) return QString();

    // 从 UTC ms 转为本地时间
    QDateTime dtLocal = QDateTime::fromMSecsSinceEpoch(ms, Qt::UTC).toLocalTime();
    if (!dtLocal.isValid()) return QString();

    QDate msgDate = dtLocal.date();
    QDate today = QDate::currentDate();

    int daysDiff = msgDate.daysTo(today); // 从 msgDate 到 today 的天数
    if (daysDiff < 0) {
        // 未来时间（可能服务器时间误差或同步问题）
        return dtLocal.toString("yyyy-MM-dd HH:mm");
    } else if (daysDiff == 0) {
        // 今天：只显示时:分
        return dtLocal.toString("HH:mm");
    } else if (daysDiff == 1) {
        return QStringLiteral("昨天");
    } else if (daysDiff == 2) {
        return QStringLiteral("前天");
    } else {
        // 更早：同年显示 MM-dd HH:mm，不同年显示 yyyy-MM-dd HH:mm
        if (msgDate.year() == today.year()) {
            return dtLocal.toString("MM-dd");
        } else {
            return dtLocal.toString("yyyy");
        }
    }
}

// 另一个重载：直接接受 QDateTime（假设为本地或有时区信息）
inline QString formatMessageTimeSmart(const QDateTime &dt) {
    if (!dt.isValid()) return QString();
    // 统一转换到本地判断
    QDateTime local = dt.toLocalTime();
    QDate msgDate = local.date();
    QDate today = QDate::currentDate();
    int daysDiff = msgDate.daysTo(today);

    if (daysDiff < 0) {
        return local.toString("yyyy-MM-dd HH:mm");
    } else if (daysDiff == 0) {
        return local.toString("HH:mm");
    } else if (daysDiff == 1) {
        return QStringLiteral("昨天");
    } else if (daysDiff == 2) {
        return QStringLiteral("前天");
    } else {
        if (msgDate.year() == today.year()) return local.toString("MM-dd");
        return local.toString("yyyy");
    }
}



// 从 "yyyy-MM-dd HH:mm" 或 "HH:mm" 构造本地 QDateTime（失败返回 currentDateTime）
inline QDateTime makeDateTime(const QString &str) {
    QDateTime dt = QDateTime::fromString(str, "yyyy-MM-dd HH:mm");
    if (dt.isValid()) return dt;
    QTime t = QTime::fromString(str, "HH:mm");
    if (t.isValid()) return QDateTime(QDate::currentDate(), t, Qt::LocalTime);
    return QDateTime::currentDateTime();
}

inline QDateTime makeDateTime(int year, int month, int day, int hour, int minute) {
    return QDateTime(QDate(year, month, day), QTime(hour, minute), Qt::LocalTime);
}



#endif // RECENT_TIME_UTLIS_H
