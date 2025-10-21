#include "autohidescrollbar.h"


#include <QWheelEvent>
#include <QMouseEvent>

AutoHideScrollbar::AutoHideScrollbar(QAbstractScrollArea* area, int timeoutMs, QObject* parent)
    : QObject(parent), m_area(area), m_timeout(timeoutMs)
{
    ensureSetup();
}

void AutoHideScrollbar::ensureSetup()
{
    if (!m_area) return;

    // 如果还没创建 timer，则创建
    if (!m_timer) {
        m_timer = new QTimer(this);
        m_timer->setSingleShot(true);
        connect(m_timer, &QTimer::timeout, this, [this]() {
            if (m_area && m_area->verticalScrollBar()) {
                m_area->verticalScrollBar()->setVisible(false);
            }
        });
    }

    // 监听滚动条数值变化
    if (m_area->verticalScrollBar()) {
        connect(m_area->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int) {
            if (!m_area) return;
            QScrollBar* sb = m_area->verticalScrollBar();
            sb->setVisible(true);
            m_timer->start(m_timeout);
        });
    }

    // 监听视口事件（滚轮、鼠标移动、触摸等）
    m_area->viewport()->installEventFilter(this);

    // 初始隐藏（如果想初始显示可去掉此行）
    if (m_area->verticalScrollBar()) {
        m_area->verticalScrollBar()->setVisible(false);
    }
}

void AutoHideScrollbar::setTimeout(int ms)
{
    m_timeout = ms;
    if (m_timer && m_timer->isActive()) {
        m_timer->start(m_timeout);
    }
}

void AutoHideScrollbar::showAndRestart()
{
    if (!m_area) return;
    if (m_area->verticalScrollBar()) m_area->verticalScrollBar()->setVisible(true);
    if (m_timer) m_timer->start(m_timeout);
}

void AutoHideScrollbar::stop(bool showImmediate)
{
    if (!m_area) return;
    if (m_timer) m_timer->stop();
    if (m_area->verticalScrollBar()) m_area->verticalScrollBar()->setVisible(showImmediate);
}

bool AutoHideScrollbar::eventFilter(QObject* obj, QEvent* ev)
{
    if (!m_area) return QObject::eventFilter(obj, ev);

    if (obj == m_area->viewport()) {
        // 关注滚轮、鼠标移动、触摸开始/更新、以及手指滚动等
        switch (ev->type()) {
        case QEvent::Wheel:
        case QEvent::MouseMove:
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::Gesture:
        {
            if (m_area->verticalScrollBar()) {
                m_area->verticalScrollBar()->setVisible(true);
            }
            if (m_timer) m_timer->start(m_timeout);
            break;
        }
        default:
            break;
        }
    }

    return QObject::eventFilter(obj, ev);
}
