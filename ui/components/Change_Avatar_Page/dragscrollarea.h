#ifndef DRAGSCROLLAREA_H
#define DRAGSCROLLAREA_H

#include <QObject>
#include <QPoint>
#include <QMouseEvent>
#include <QScrollBar>
#include <QCursor>
#include <qDebug>
#include <QScrollArea>

class DragScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    DragScrollArea(QWidget* parent = nullptr);
    ~DragScrollArea() override = default;


private:
    bool m_dragging = false;    //是否被拖动状态
    QPoint m_lastDragPos;       //最后拖动后图片的位置

signals:
    void zoomChanged(int num);


    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
};

#endif // DRAGSCROLLAREA_H
