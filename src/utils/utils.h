#ifndef UTILS_H
#define UTILS_H

#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QDateTime>
#include <QString>


// 帮助函数：先缩放再圆角
inline QPixmap scaledRoundedPixmap(const QPixmap &src, const QSize &targetSize, int radius) {
    // 1. 缩放到目标尺寸（这里用填充方式保证铺满，再裁剪到圆角）
    QPixmap scaled = src.scaled(
        targetSize,
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation
        );

    // 2. 生成一个同样大小、带透明背景的画布
    QPixmap dest(targetSize);
    dest.fill(Qt::transparent);

    // 3. 在画布上用圆角路径做裁剪蒙版
    QPainter painter(&dest);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath path;
    path.addRoundedRect(
        QRectF(0, 0, targetSize.width(), targetSize.height()),
        radius, radius
        );
    painter.setClipPath(path);

    // 4. 将缩放后的图绘制到裁剪区域里
    //    因为是“填充”缩放，可能超出了一点，需要居中绘制
    QPointF offset(
        (targetSize.width()  - scaled.width())  / 2.0,
        (targetSize.height() - scaled.height()) / 2.0
        );
    painter.drawPixmap(offset, scaled);
    return dest;
}












inline qint64 makeTimestampMs(int year, int month, int day, int hour, int minute) {
    // 假设用户输入的是本地时间（LocalTime）
    QDate d(year, month, day);
    QTime t(hour, minute, 0); // 秒置为0
    QDateTime dt(d, t, Qt::LocalTime);
    // 转为 UTC epoch 毫秒存储
    return dt.toUTC().toMSecsSinceEpoch();
}

inline QDateTime dateTimeFromMs(qint64 ms) {
    // 从存储的 UTC 毫秒转回本地时间用于显示
    return QDateTime::fromMSecsSinceEpoch(ms, Qt::UTC).toLocalTime();
}

inline QString formatMsToYMDHM(qint64 ms) {
    QDateTime dt = dateTimeFromMs(ms);
    return dt.toString("yyyy-MM-dd HH:mm"); // 精确到分钟
}


// 示例
// qint64 ts = makeTimestampMs(2025, 10, 13, 14, 30);   // 用户本地时间 2025-10-13 14:30 -> 存为 UTC ms
// QString disp = formatMsToYMDHM(ts);                  // 格式化回显示字符串（本地时区）







#endif // UTILS_H
