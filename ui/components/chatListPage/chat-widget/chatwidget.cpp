#include "chatwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListView>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QTimer>

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent), m_lastViewportWidth(-1) {
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(6);

    // List view for messages
    m_listView = new QListView;
    m_model = new ChatModel(this);
    m_delegate = new ChatDelegate(this);

    m_listView->setModel(m_model);
    m_listView->setItemDelegate(m_delegate);
    m_listView->setVerticalScrollMode(QListView::ScrollPerPixel);
    m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setSelectionMode(QAbstractItemView::NoSelection);
    m_listView->setFocusPolicy(Qt::NoFocus);
    m_listView->setStyleSheet(R"(
    QListView {
        background-color: #f0f0f0;
        border: none;
        outline: none;
    }

    /* 垂直滚动条 */
    QScrollBar:vertical {
        border: none;
        background: #f0f0f0;
        width: 7px;
        margin: 0px;
    }

    /* 垂直滚动条手柄 */
    QScrollBar::handle:vertical {
        background: #c0c0c0;
        border-radius: 3px;
        min-height: 30px;
        margin: 0px;
    }

    /* 垂直滚动条手柄悬停效果 */
    QScrollBar::handle:vertical:hover {
        background: #a8a8a8;
    }

    /* 垂直滚动条页面区域 */
    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
        background: #f0f0f0;
    }

    /* 垂直滚动条上下箭头 */
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
        border: none;
        background: #f0f0f0;
        height: 0px;
        subcontrol-position: center;
    }

    /* 滚动条角部 */
    QScrollBar::corner {
        background: #f0f0f0;
        border: none;
    }

    /* 确保滚动条按钮不显示 */
    QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {
        border: none;
        background: none;
        width: 0px;
        height: 0px;
    }
)");
    root->addWidget(m_listView, 1);

    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(500); // 50ms debounce
    connect(m_updateTimer, &QTimer::timeout, this, &ChatWidget::updateDelegateWidth);

    // Connect model changes to scroll to bottom
    connect(m_model, &ChatModel::rowsInserted, this, &ChatWidget::scrollToBottom);

    // Add example messages
    for (int i = 0; i < 1; ++i) {
        addMessage(false,"D:\\Documents\\Qt\\MyClient\\src\\resources\\picture\\avatar\\3.jpg","AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA123");
        addMessage(false,"D:\\Documents\\Qt\\MyClient\\src\\resources\\picture\\avatar\\4.jpg","对方消息：短文本会按文本长度显示，不会被强制换行（若能单行完整显示则不换行）。");
        addMessage(true,"://picture/avatar/2.jpg","我的消息：短消息也是单行显示且靠右。");
        addMessage(false,"://picture/avatar/1.jpg","我的消息：短消息也是单行显示且靠右。");

        addMessage(false,"://picture/avatar/1.jpg","这是一条非常长的对方消息示例，用来测试当文本宽度超过可用宽度时，气泡会被拉满并自动换行。窗口缩放时，气泡会根据 scroll 区域的可用宽度重新设置宽度，从而触发文本换行和高度自适应。");
    }

    // Initial update
    QTimer::singleShot(0, this, [this](){
        updateDelegateWidth();
        scrollToBottom();
    });
}

void ChatWidget::addMessage(bool isSelf,QString avatar_url,const QString &text) {
    m_model->addMessage(text, isSelf,avatar_url);
}

void ChatWidget::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
    m_updateTimer->start();
}

void ChatWidget::updateDelegateWidth() {
    int viewportW = m_listView->viewport()->width();
    if (qAbs(viewportW - m_lastViewportWidth) <= 1) return;
    m_lastViewportWidth = viewportW;

    const int avatarAreaWidth = 56; // avatar(40) + spacing
    int avail = viewportW - avatarAreaWidth;
    if (avail < 50) avail = 50;

    m_delegate->updateWidth(avail);
    m_listView->doItemsLayout(); // Force relayout
}

void ChatWidget::scrollToBottom() {
    QTimer::singleShot(0, this, [this](){
        m_listView->scrollToBottom();
    });
}
