#include "chatbubble.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextOption>
#include <QFontMetrics>

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

    const int gap = 36; // 左右额外留白（整体布局的安全边距）
    const int bubblePaddingLR = 12 + 12; // 气泡内边距（和上面 bubbleLayout 一致）
    const int minBubbleW = 60; // 最小气泡宽度

    int availWidth = parentWidth - gap;
    if (availWidth < minBubbleW) availWidth = minBubbleW;

    QFontMetrics fm(m_textBrowser->font());
    QString text = m_textBrowser->toPlainText();

    // 计算文本固有宽度（单行测量）
    int textWidth = fm.horizontalAdvance(text);

    // 判断是否需要换行模式
    bool shouldWrap = (textWidth + bubblePaddingLR) > availWidth;

    // 决定模式
    Mode newMode = shouldWrap ? Mode::Wrap : Mode::SingleLine;

    // 计算目标宽度
    int targetWidth;
    if (newMode == Mode::SingleLine) {
        // 单行模式：文本宽度 + 内边距
        targetWidth = textWidth + bubblePaddingLR;
        if (targetWidth < minBubbleW) targetWidth = minBubbleW;
    } else {
        // 换行模式：使用可用宽度
        targetWidth = availWidth;
    }

    // 只有当模式改变或宽度变化较大时才更新
    bool needChange = false;
    if (newMode != m_mode) {
        needChange = true;
    } else if (qAbs(targetWidth - m_lastWidth) > 10) { // 10px变化阈值
        needChange = true;
    }

    if (!needChange) return;

    // 应用变更
    if (newMode == Mode::SingleLine) {
        // 单行模式
        m_bubble->setMinimumWidth(0);
        m_bubble->setMaximumWidth(QWIDGETSIZE_MAX);
        m_bubble->setFixedWidth(targetWidth);
        // 设置文本浏览器使用固定宽度
        m_textBrowser->setFixedWidth(targetWidth - bubblePaddingLR);
        m_mode = Mode::SingleLine;
    } else {
        // 换行模式
        m_bubble->setMinimumWidth(targetWidth);
        m_bubble->setMaximumWidth(targetWidth);
        // 设置文本浏览器使用固定宽度
        m_textBrowser->setFixedWidth(targetWidth - bubblePaddingLR);
        m_mode = Mode::Wrap;
    }

    m_lastWidth = targetWidth;

    // 强制文本浏览器重新计算尺寸
    m_textBrowser->document()->setTextWidth(m_textBrowser->width());
    int idealHeight = int(m_textBrowser->document()->size().height() + 0.5);
    m_textBrowser->setFixedHeight(idealHeight);

    // 如果需要，avatar 的高度也可以根据气泡高度垂直居中（由布局处理）
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
    m_textBrowser->setAlignment(Qt::AlignRight);
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
