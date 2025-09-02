#include "chatbubble.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextOption>
#include <QFontMetrics>
#include <QTextCursor>
#include <QTextBlockFormat>

ChatBubbleBase::ChatBubbleBase(const QString &text, QWidget *parent)
    : QWidget(parent), m_lastWidth(-1), m_mode(Mode::Unknown), m_avatar(nullptr)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // 使用 QTextBrowser
    m_textBrowser = new QTextBrowser;
    m_textBrowser->setPlainText(text);
    m_textBrowser->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_textBrowser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_textBrowser->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textBrowser->setFrameShape(QFrame::NoFrame);

    // 设置样式以精确控制尺寸
    m_textBrowser->setStyleSheet(
        "QTextBrowser {"
        "   background: transparent;"
        "   border: none;"
        "   padding: 0px;"
        "   margin: 0px;"
        "}"
        );

    // 移除文档边距
    m_textBrowser->document()->setDocumentMargin(0);
    // 设置文档使用理想宽度计算高度
    m_textBrowser->document()->setTextWidth(-1);

    m_bubble = new QFrame;
    QVBoxLayout *bubbleLayout = new QVBoxLayout(m_bubble);
    bubbleLayout->setContentsMargins(12, 8, 12, 8); // 内边距：左右12，上下8
    bubbleLayout->addWidget(m_textBrowser);

    m_bubble->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_bubble->setMinimumWidth(0);
}

ChatBubbleBase::~ChatBubbleBase() = default;

void ChatBubbleBase::updateBubbleWidth(int parentWidth) {
    if (parentWidth <= 0) return;

    const int gap = parentWidth * 0.1; // 左右额外留白（整体布局的安全边距）
    const int bubblePaddingLR = 12 + 12; // 气泡内边距（和上面 bubbleLayout 一致）
    const int minBubbleW = 30; // 最小气泡宽度

    int availWidth = parentWidth - gap;
    if (availWidth < minBubbleW) availWidth = minBubbleW;

    // 使用文档默认字体来测量，确保与 QTextBrowser 显示一致
    QFont docFont = m_textBrowser->document()->defaultFont();
    QFontMetrics fm(docFont);

    QString text = m_textBrowser->toPlainText();
    // 去掉可能的回车字符，仅按 '\n' 分行
    text.replace('\r', QString());
    QStringList lines = text.split('\n', QString::KeepEmptyParts);

    // 计算每一行的像素宽度，取最大值
    int maxLineWidth = 0;
    for (const QString &line : lines) {
        int w = fm.horizontalAdvance(line);
        if (w > maxLineWidth) maxLineWidth = w;
    }

    // 当文本为空（或只有空行）时，确保有最小宽度
    if (maxLineWidth <= 0) maxLineWidth = fm.horizontalAdvance(QString(" ")); // 至少一个空格宽度

    // 期望宽度（包含气泡内侧左右 padding）
    int desiredWidth = maxLineWidth + bubblePaddingLR;
    if (desiredWidth < minBubbleW) desiredWidth = minBubbleW;

    // 如果最长行超过可用宽度，则进入换行模式并使用可用宽度
    bool needWrapToAvail = (desiredWidth > availWidth);

    int targetWidth;
    if (needWrapToAvail) {
        targetWidth = availWidth;
        m_mode = Mode::Wrap;
    } else {
        targetWidth = desiredWidth;
        m_mode = Mode::SingleLine; // 语义上：按最长行宽显示（虽有多行，但不强制整行换成可用宽度）
    }

    // 只有当宽度变化明显时才更新（避免频繁刷新）
    if (m_lastWidth <= 0 || qAbs(targetWidth - m_lastWidth) > 4 || m_mode != m_mode) {
        // 应用宽度到气泡和文本控件
        if (needWrapToAvail) {
            m_bubble->setMinimumWidth(targetWidth);
            m_bubble->setMaximumWidth(targetWidth);
        } else {
            m_bubble->setMinimumWidth(0);
            m_bubble->setMaximumWidth(QWIDGETSIZE_MAX);
            m_bubble->setFixedWidth(targetWidth);
        }

        m_textBrowser->setFixedWidth(targetWidth - bubblePaddingLR);
        m_lastWidth = targetWidth;
    }

    // 把当前 alignment 应用到文档中每个 block，确保每一行都对齐一致
    {
        Qt::Alignment align = m_textBrowser->alignment();
        QTextCursor cursor(m_textBrowser->document());
        cursor.beginEditBlock();
        cursor.select(QTextCursor::Document);
        QTextBlockFormat bf;
        if (align & Qt::AlignRight) bf.setAlignment(Qt::AlignRight);
        else if (align & Qt::AlignHCenter) bf.setAlignment(Qt::AlignHCenter);
        else bf.setAlignment(Qt::AlignLeft);
        cursor.mergeBlockFormat(bf);
        cursor.clearSelection();
        cursor.endEditBlock();
    }

    // 强制文档使用文本控件当前宽度来计算布局高度
    m_textBrowser->document()->setTextWidth(m_textBrowser->width());
    int idealHeight = int(m_textBrowser->document()->size().height() + 0.5);
    m_textBrowser->setFixedHeight(idealHeight);

    updateGeometry();
    m_bubble->updateGeometry();
}


///////////// LeftBubble /////////////
LeftBubble::LeftBubble(const QString &text, QWidget *parent)
    : ChatBubbleBase(text, parent)
{
    m_textBrowser->setAlignment(Qt::AlignLeft);
    m_bubble->setObjectName("bubbleLeft");
    m_bubble->setStyleSheet("#bubbleLeft{ background:#ffffff; border-radius:10px; }");

    // 创建头像（圆形占位）
    m_avatar = new QLabel;
    m_avatar->setFixedSize(40, 40);
    m_avatar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_avatar->setStyleSheet("border-radius:20px; background: #bbbbbb;");

    setupAlignment();
}

void LeftBubble::setupAlignment() {
    QHBoxLayout *mainLay = new QHBoxLayout(this);
    mainLay->setContentsMargins(0,0,0,0);
    mainLay->setSpacing(8);
    mainLay->addWidget(m_avatar, 0, Qt::AlignTop);
    mainLay->addWidget(m_bubble);
    mainLay->addStretch();
}

///////////// RightBubble /////////////
RightBubble::RightBubble(const QString &text, QWidget *parent)
    : ChatBubbleBase(text, parent)
{
    m_textBrowser->setAlignment(Qt::AlignLeft);
    m_bubble->setObjectName("bubbleRight");
    m_bubble->setStyleSheet("#bubbleRight{ background:#daf8c6; border-radius:10px; }");

    // 创建头像（圆形占位）
    m_avatar = new QLabel;
    m_avatar->setFixedSize(40, 40);
    m_avatar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_avatar->setStyleSheet("border-radius:20px; background: #66bb6a;");

    setupAlignment();
}

void RightBubble::setupAlignment() {
    QHBoxLayout *mainLay = new QHBoxLayout(this);
    mainLay->setContentsMargins(0,0,0,0);
    mainLay->setSpacing(8);
    mainLay->addStretch();
    mainLay->addWidget(m_bubble);
    mainLay->addWidget(m_avatar, 0, Qt::AlignTop);
}
