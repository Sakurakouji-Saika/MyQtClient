#include "chatwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QLayoutItem>
#include <QTimer>

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent), m_lastViewportWidth(-1)
{
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(8,8,8,8);
    root->setSpacing(6);

    // Scroll area + messages container
    m_scroll = new QScrollArea;
    m_scroll->setWidgetResizable(true);
    // m_scroll->setContentsMargins(10,10,10,10);

    m_messagesWidget = new QWidget;
    m_messagesLayout = new QVBoxLayout(m_messagesWidget);
    m_messagesLayout->setAlignment(Qt::AlignTop);
    m_messagesLayout->setSpacing(6);
    m_messagesLayout->setContentsMargins(10,10,10,10); // 关键：去掉底部多余空隙
    m_messagesWidget->setLayout(m_messagesLayout);
    m_scroll->setWidget(m_messagesWidget);
    root->addWidget(m_scroll, 1);

    // // Input area
    // QHBoxLayout *inputLay = new QHBoxLayout;
    // m_input = new QLineEdit;
    // m_input->setPlaceholderText("输入消息，按 Enter 发送（默认我方）");
    // QPushButton *sendMe = new QPushButton("发送(我)");
    // QPushButton *sendOther = new QPushButton("发送(对方)");
    // inputLay->addWidget(m_input, 1);
    // inputLay->addWidget(sendMe);
    // inputLay->addWidget(sendOther);
    // root->addLayout(inputLay);

    // connect(m_input, &QLineEdit::returnPressed, this, [this](){
    //     QString t = m_input->text().trimmed();
    //     if (!t.isEmpty()) {
    //         addMessage(t, true);
    //         m_input->clear();
    //     }
    // });
    // connect(sendMe, &QPushButton::clicked, this, [this](){
    //     QString t = m_input->text().trimmed();
    //     if (!t.isEmpty()) {
    //         addMessage(t, true);
    //         m_input->clear();
    //     }
    // });
    // connect(sendOther, &QPushButton::clicked, this, [this](){
    //     QString t = m_input->text().trimmed();
    //     if (!t.isEmpty()) {
    //         addMessage(t, false);
    //         m_input->clear();
    //     }
    // });

    // 创建 debounce 定时器（用于 resize 去抖）
    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(1000); // 50ms 去抖
    connect(m_updateTimer, &QTimer::timeout, this, &ChatWidget::updateAllBubbleWidths);

    // 示例消息（保持你原来示例）
    for (int i = 0; i < 500; ++i) {
        addMessage("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA123", false);
        addMessage("对方消息：短文本会按文本长度显示，不会被强制换行（若能单行完整显示则不换行）。", false);
        addMessage("我的消息：短消息也是单行显示且靠右。", true);
        addMessage("这是一条非常长的对方消息示例，用来测试当文本宽度超过可用宽度时，气泡会被拉满并自动换行。窗口缩放时，气泡会根据 scroll 区域的可用宽度重新设置宽度，从而触发文本换行和高度自适应。", false);
    }

    // 初次强制一次更新并确保滚到底（先更新宽度，再确保最后一条可见，然后滚动）
    QTimer::singleShot(0, this, [this](){
        updateAllBubbleWidths();

        // 找最后一个实际 widget（跳过可能的空项）
        QWidget *lastWidget = nullptr;
        for (int i = m_messagesLayout->count() - 1; i >= 0; --i) {
            QLayoutItem *it = m_messagesLayout->itemAt(i);
            if (it && it->widget()) { lastWidget = it->widget(); break; }
        }

        QTimer::singleShot(0, this, [this, lastWidget]() {
            if (lastWidget) {
                m_scroll->ensureWidgetVisible(lastWidget, 0, 0); // 确保最后一条完全可见
            }
            QScrollBar *bar = m_scroll->verticalScrollBar();
            bar->setValue(bar->maximum()); // 强制滚到底
        });
    });
}

void ChatWidget::addMessage(const QString &text, bool isSelf) {
        ChatBubbleBase *b = isSelf ? static_cast<ChatBubbleBase*>(new RightBubble(text))
                                    : static_cast<ChatBubbleBase*>(new LeftBubble(text));
        m_messagesLayout->addWidget(b);

        // 立即更新新消息的宽度，考虑头像占用宽度
        const int avatarAreaWidth = 56; // avatar(40) + 间距/边距等预留
        int avail = m_scroll->viewport()->width() - avatarAreaWidth;
        if (avail < 50) avail = 50;
        b->updateBubbleWidth(avail);

        // 滚动到底部 - 使用较长的延迟确保布局完成
        QTimer::singleShot(100, this, [this](){
            QScrollBar *bar = m_scroll->verticalScrollBar();
            bar->setValue(bar->maximum());
        });
}

void ChatWidget::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
    // 使用去抖定时器，避免频繁更新
    m_updateTimer->start();
}

void ChatWidget::updateAllBubbleWidths() {
    int viewportW = m_scroll->viewport()->width();
    if (qAbs(viewportW - m_lastViewportWidth) <= 1) return;
    m_lastViewportWidth = viewportW;

    // 计算 viewport 在 messagesWidget 中的可见矩形
    QPoint vpTopLeft = m_scroll->viewport()->mapTo(m_messagesWidget, QPoint(0,0));
    QRect visibleRect(vpTopLeft, m_scroll->viewport()->size());

    // 给上下缓冲（预先布局看起来更平滑）
    const int buffer = 200;
    visibleRect.adjust(0, -buffer, 0, buffer);

    const int avatarAreaWidth = 56;

    for (int i = 0; i < m_messagesLayout->count(); ++i) {
        QLayoutItem *it = m_messagesLayout->itemAt(i);
        if (!it) continue;
        QWidget *w = it->widget();
        if (!w) continue;

        // 只处理可见或接近可见的 widget
        if (!w->geometry().intersects(visibleRect)) continue;

        ChatBubbleBase *b = qobject_cast<ChatBubbleBase*>(w);
        if (b) {
            int avail = viewportW - avatarAreaWidth;
            if (avail < 50) avail = 50;
            b->updateBubbleWidth(avail);
        }
    }
}

