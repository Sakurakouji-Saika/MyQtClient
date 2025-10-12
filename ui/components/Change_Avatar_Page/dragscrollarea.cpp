#include "dragscrollarea.h"

DragScrollArea::DragScrollArea(QWidget* parent): QScrollArea(parent) {
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 关闭水平和垂直滚动条

    setWidgetResizable(true);
    // 允许容器内可以自动调整大小
}



//  mousePressEvent::鼠标按下事件
void DragScrollArea::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && widget()) {
        m_dragging = true;
        m_lastDragPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    QScrollArea::mousePressEvent(event);
}

void DragScrollArea::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging && widget()) {
        const QPoint currentPos = event->pos();
        const QPoint delta = currentPos - m_lastDragPos;
        m_lastDragPos = currentPos;

        // 更新滚动条位置
        QScrollBar* hBar = horizontalScrollBar();
        QScrollBar* vBar = verticalScrollBar();

        hBar->setValue(hBar->value() - delta.x());
        vBar->setValue(vBar->value() - delta.y());



        event->accept();
        return;
    }

    QScrollArea::mouseMoveEvent(event);
}

void DragScrollArea::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_dragging && event->button() == Qt::LeftButton) {
        m_dragging = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }

    QScrollArea::mouseReleaseEvent(event);
}

void DragScrollArea::leaveEvent(QEvent* event)
{
    // 如果鼠标移出控件且正在拖动，重置状态
    if (m_dragging) {
        m_dragging = false;
        setCursor(Qt::ArrowCursor);
    }

    QScrollArea::leaveEvent(event);
}

void DragScrollArea::wheelEvent(QWheelEvent *event)
{
    // 优先使用 angleDelta（Qt5+）
    int dy = event->angleDelta().y(); // >0 向上，<0 向下
    // qDebug() << "dy:" << dy;

    // 正确判断方向：dy>0 表示向上（放大），dy<0 表示向下（缩小）
    if (dy > 0) {
        emit zoomChanged(3);
    } else if (dy < 0) {
        emit zoomChanged(-3);
    }

    // 如果希望按“格”计数（每格通常为120）
    // int steps = dy / 120;

    event->accept(); // 或者根据需要调用 QWidget::wheelEvent(event);
}
