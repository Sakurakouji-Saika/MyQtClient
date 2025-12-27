#include "chatdelegate.h"
#include <QApplication>
#include <QPainter>
#include <QFontMetrics>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QStyleOptionViewItem>

ChatDelegate::ChatDelegate(QObject *parent)
    : QStyledItemDelegate(parent), m_availableWidth(300) {
}

void ChatDelegate::updateWidth(int width) {
    m_availableWidth = width;
    m_sizeCache.clear(); // Clear cache when width changes
}

QSize ChatDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {

    QString text = index.data(TextRole).toString();
    bool isSelf = index.data(IsSelfRole).toBool();
    QString avatar_url = index.data(avatarUrlRole).toString();


    int baseWidth = option.rect.width();
    if (baseWidth <= 50) {
        baseWidth = m_availableWidth;
    }

    QString cacheKey = QString::number(baseWidth) + "|" + text + (isSelf ? "_right" : "_left");
    if (m_sizeCache.contains(cacheKey)) {
        return m_sizeCache.value(cacheKey);
    }

    //气泡的10% 留空
    int qblk = baseWidth*0.1;


    // 计算可用宽度（考虑边距和头像）
    const int sideMargin = 14; // 左右边距
    const int avatarWidth = 40; // 头像宽度
    const int avatarBubbleGap = 8; // 头像与气泡间距

    // 可用宽度 = 基准宽度 - 左右边距 - 头像宽度 - 头像气泡间距
    int availWidth = baseWidth - 2 * sideMargin - avatarWidth - avatarBubbleGap - qblk;
    if (availWidth < 30) availWidth = 30; // 最小气泡宽度

    QFont font("Microsoft YaHei", option.font.pointSize());
    QFontMetrics fm(font);

    // Calculate text width
    text.replace('\r', QString());

    QStringList lines = text.split('\n', Qt::KeepEmptyParts);

    int maxLineWidth = 0;
    for (const QString &line : lines) {
        int w = fm.horizontalAdvance(line);
        if (w > maxLineWidth) maxLineWidth = w;
    }

    if (maxLineWidth <= 0) maxLineWidth = fm.horizontalAdvance(QString(" "));

    const int bubblePaddingLR = 24; // 12 + 12
    int desiredWidth = maxLineWidth + bubblePaddingLR;
    if (desiredWidth < 30) desiredWidth = 30; // 最小气泡宽度

    bool needWrapToAvail = (desiredWidth > availWidth);
    int targetWidth = needWrapToAvail ? availWidth : desiredWidth;

    // Calculate text height
    QTextDocument doc;
    doc.setDefaultFont(font);
    doc.setPlainText(text);

    if (needWrapToAvail) {
        doc.setTextWidth(targetWidth - bubblePaddingLR);
    } else {
        doc.setTextWidth(-1);
    }

    int textHeight = doc.size().height();
    int bubbleHeight = textHeight + 16;

    int totalHeight = bubbleHeight + 11;

    QSize size(targetWidth, totalHeight);
    m_sizeCache.insert(cacheKey, size);

    return size;
}

void ChatDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    // --- 取数据 ---
    const QString text = index.data(TextRole).toString();
    const bool isSelf = index.data(IsSelfRole).toBool();
    const QString avatar_url = index.data(avatarUrlRole).toString();

    QRect rect = option.rect;
    QStyleOptionViewItem opt = option;
    opt.displayAlignment = Qt::AlignLeft;

    // 选中背景保持原样
    if (opt.state & QStyle::State_Selected)
        painter->fillRect(rect, opt.palette.highlight());

    // --- 尺寸/布局参数 ---
    QSize bubbleSize = sizeHint(option, index);
    const int bubbleWidth = qMax(0, bubbleSize.width());
    // 原来的实现里减了 11，保持行为一致（如果想改，可在 sizeHint 里统一处理）
    const int bubbleHeight = qMax(0, bubbleSize.height() - 11);

    const int sideMargin = 14;
    const int avatarSize = 40;
    const int avatarGap = 12; // 头像与气泡间距

    // 把 rect.height() 与 bubbleHeight 的差值一分为二，topOffset 放到上方
    const int totalGap = rect.height() - bubbleHeight;
    const int topOffset = (totalGap > 0) ? (totalGap / 2) : 0;
    const int top = rect.top() + topOffset;

    QRect avatarRect;
    QRect bubbleRect;
    if (isSelf) {
        avatarRect = QRect(rect.right() - sideMargin - avatarSize, top, avatarSize, avatarSize);
        bubbleRect = QRect(avatarRect.left() - avatarGap - bubbleWidth, top, bubbleWidth, bubbleHeight);
    } else {
        avatarRect = QRect(rect.left() + sideMargin, top, avatarSize, avatarSize);
        bubbleRect = QRect(avatarRect.right() + avatarGap, top, bubbleWidth, bubbleHeight);
    }

    // --- 绘制气泡背景（抗锯齿 + 颜色） ---
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    const QColor bubbleColor = isSelf ? QColor("#0099FF") : Qt::white;
    QPainterPath bubblePath;
    bubblePath.addRoundedRect(bubbleRect, 10, 10);
    painter->fillPath(bubblePath, bubbleColor);

    // --- 绘制头像（圆形裁剪） ---
    QPixmap avatar;
    if (!avatar_url.isEmpty())
        avatar = QPixmap(avatar_url);

    if (!avatar.isNull()) {
        const QPixmap scaled = avatar.scaled(avatarRect.size(),
                                             Qt::KeepAspectRatioByExpanding,
                                             Qt::SmoothTransformation);
        const int sx = (scaled.width() - avatarRect.width()) / 2;
        const int sy = (scaled.height() - avatarRect.height()) / 2;
        const QRect srcRect(sx, sy, avatarRect.width(), avatarRect.height());

        painter->save();
        QPainterPath clip;
        clip.addEllipse(avatarRect);
        painter->setClipPath(clip);
        painter->drawPixmap(avatarRect, scaled, srcRect);
        painter->restore();
    } else {
        // 占位圆
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(isSelf ? QColor("#66bb6a") : QColor("#bbbbbb"));
        painter->drawEllipse(avatarRect);
        painter->restore();
    }

    // --- 绘制文本 ---
    const QRect textRect = bubbleRect.adjusted(12, 8, -12, -8);

    // 减少重复创建 font：使用 opt.font 为基础并强制设置字体家族（保留原来视觉）
    QFont font = opt.font;
    font.setFamily("Microsoft YaHei");
    painter->setFont(font);
    QFontMetrics fm(font);

    painter->setPen(isSelf ? Qt::white : Qt::black);

    // 快速判断是否单行能容纳，否则使用 QTextDocument 自动换行
    const int textWidth = fm.horizontalAdvance(text);
    if (textWidth <= textRect.width()) {
        const QPoint textPos = isSelf
                                   ? QPoint(textRect.right() - textWidth, textRect.top() + fm.ascent() + 3)
                                   : QPoint(textRect.left(), textRect.top() + fm.ascent() + 3);
        painter->drawText(textPos, text);
    } else {
        QTextDocument doc;
        doc.setDefaultFont(font);
        doc.setPlainText(text);
        doc.setTextWidth(qMax(1, textRect.width())); // 避免 0 宽引发问题

        painter->save();
        painter->translate(textRect.topLeft());
        QAbstractTextDocumentLayout::PaintContext ctx;
        ctx.palette.setColor(QPalette::Text, isSelf ? Qt::white : Qt::black);
        doc.documentLayout()->draw(painter, ctx);
        painter->restore();
    }

    painter->restore();
}
