#ifndef UTILS_H
#define UTILS_H

#include <QPixmap>
#include <QPainter>
#include <QPainterPath>

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


#endif // UTILS_H
