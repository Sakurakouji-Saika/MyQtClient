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

    // Use cache if available
    QString cacheKey = text + (isSelf ? "_right" : "_left");
    if (m_sizeCache.contains(cacheKey)) {
        return m_sizeCache.value(cacheKey);
    }

    // 计算可用宽度（考虑边距和头像）
    const int sideMargin = 14; // 左右边距
    const int avatarWidth = 40; // 头像宽度
    const int avatarBubbleGap = 8; // 头像与气泡间距

    // 可用宽度 = 总宽度 - 左右边距 - 头像宽度 - 头像气泡间距
    int availWidth = m_availableWidth - 2 * sideMargin - avatarWidth - avatarBubbleGap;
    if (availWidth < 30) availWidth = 30; // 最小气泡宽度

    QFont font("Microsoft YaHei", option.font.pointSize());
    QFontMetrics fm(font);

    // Calculate text width
    text.replace('\r', QString());
    QStringList lines = text.split('\n', QString::KeepEmptyParts);

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

    // 关键修改：对于短文本，不使用固定宽度，让文本自然布局
    if (needWrapToAvail) {
        doc.setTextWidth(targetWidth - bubblePaddingLR);
    } else {
        doc.setTextWidth(-1); // 使用自然宽度
    }

    int textHeight = doc.size().height();
    int bubbleHeight = textHeight + 16; // 8 + 8 padding

    // Total item height (bubble + margins)
    int totalHeight = bubbleHeight + 6; // spacing

    QSize size(targetWidth, totalHeight);
    m_sizeCache.insert(cacheKey, size);

    return size;
}

void ChatDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    painter->save();

    QString text = index.data(TextRole).toString();
    bool isSelf = index.data(IsSelfRole).toBool();
    QString avatar_url = index.data(avatarUrlRole).toString();

    QRect rect = option.rect;
    QStyleOptionViewItem opt = option;
    opt.displayAlignment = Qt::AlignLeft;

    // Draw background if selected
    if (opt.state & QStyle::State_Selected) {
        painter->fillRect(rect, opt.palette.highlight());
    }

    // Calculate bubble dimensions
    QSize bubbleSize = sizeHint(option, index);
    int bubbleWidth = bubbleSize.width();
    int bubbleHeight = bubbleSize.height() - 6; // Remove spacing

    // 定义边距
    const int sideMargin = 14;

    QRect bubbleRect;
    QRect avatarRect;

    if (isSelf) {
        // Right bubble (self)
        // 头像位置：右侧边距14px
        avatarRect = QRect(rect.right() - sideMargin - 40, rect.top(), 40, 40);
        // 气泡位置：头像左侧，与头像间隔8px
        bubbleRect = QRect(avatarRect.left() - 8 - bubbleWidth, rect.top(), bubbleWidth, bubbleHeight);
    } else {
        // Left bubble (other)
        // 头像位置：左侧边距14px
        avatarRect = QRect(rect.left() + sideMargin, rect.top(), 40, 40);
        // 气泡位置：头像右侧，与头像间隔8px
        bubbleRect = QRect(avatarRect.right() + 8, rect.top(), bubbleWidth, bubbleHeight);
    }

    // Draw bubble background
    painter->setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(bubbleRect, 10, 10);

    if (isSelf) {
        painter->fillPath(path, QColor("#0099FF")); // Right bubble color
    } else {
        painter->fillPath(path, Qt::white); // Left bubble color
    }

    // ---------------------------
    // Draw avatar as a circular image
    // ---------------------------
    // 注意：资源前缀使用 ":/..."
    QPixmap avatar(avatar_url);

    // 开启平滑缩放提示以提高绘制质量
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    if (!avatar.isNull()) {
        // 将图片缩放到比目标稍大（保持纵横比），以便居中裁剪
        QPixmap scaled = avatar.scaled(avatarRect.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        // 计算从 scaled 中取哪一块作为 source（居中裁切）
        int sx = (scaled.width() - avatarRect.width()) / 2;
        int sy = (scaled.height() - avatarRect.height()) / 2;
        QRect srcRect(sx, sy, avatarRect.width(), avatarRect.height());

        // 使用圆形裁剪后绘制（保证边缘平滑）
        painter->save();
        QPainterPath clipPath;
        clipPath.addEllipse(avatarRect);
        painter->setClipPath(clipPath);
        painter->drawPixmap(avatarRect, scaled, srcRect);
        painter->restore();
    } else {
        // 图片加载失败 -> 使用原先的颜色回退（保证不会空白）
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(isSelf ? QColor("#66bb6a") : QColor("#bbbbbb"));
        painter->drawEllipse(avatarRect);
        painter->restore();
    }

    // Draw text
    QRect textRect = bubbleRect.adjusted(12, 8, -12, -8);

    painter->setPen(isSelf ? Qt::white : Qt::black);
    QFont font("Microsoft YaHei", opt.font.pointSize());
    painter->setFont(font);

    // 对于短文本，直接使用 QPainter 绘制，避免 QTextDocument 的问题
    QFontMetrics fm(opt.font);
    int textWidth = fm.horizontalAdvance(text);

    // 如果文本宽度小于文本区域的宽度，直接绘制
    if (textWidth <= textRect.width()) {
        // 设置文本对齐方式
        Qt::Alignment alignment = isSelf ? Qt::AlignRight : Qt::AlignLeft;

        // 计算文本绘制位置
        QPoint textPos;
        if (isSelf) {
            textPos = QPoint(textRect.right() - textWidth, textRect.top() + fm.ascent() + 3);
        } else {
            textPos = QPoint(textRect.left(), textRect.top() + fm.ascent() + 3);
        }

        painter->drawText(textPos, text);
    } else {
        // 对于长文本，使用 QTextDocument 进行换行
        QTextDocument doc;
        QFont font("Microsoft YaHei", opt.font.pointSize());
        doc.setDefaultFont(font);
        doc.setPlainText(text);
        doc.setTextWidth(textRect.width());

        // 设置文本颜色（自己发送的为白色，他人发送的为黑色）
        if (isSelf) {
            doc.setDefaultStyleSheet("body { color: white; }");
        } else {
            doc.setDefaultStyleSheet("body { color: black; }");
        }

        painter->save();
        painter->translate(textRect.topLeft());
        // 确保使用正确的文本颜色绘制
        QAbstractTextDocumentLayout::PaintContext ctx;
        ctx.palette.setColor(QPalette::Text, isSelf ? Qt::white : Qt::black);
        doc.documentLayout()->draw(painter, ctx);
        painter->restore();
    }

    painter->restore();
}
