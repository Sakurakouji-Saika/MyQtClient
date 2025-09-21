// hoverbutton.h
#ifndef HOVERBUTTON_H
#define HOVERBUTTON_H

#include <QIcon>
#include <QEvent>
#include <QObject>
#include <QPushButton>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QEnterEvent>
#endif

class HoverButton : public QPushButton
{
    Q_OBJECT
public:
    explicit HoverButton(QWidget *parent = nullptr);
    void setNormalIcon(const QIcon&);
    void setHoverIcon(const QIcon&);

protected:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *e) override;  // Qt 6 使用 QEnterEvent
#else
    void enterEvent(QEvent *e) override;       // Qt 5 使用 QEvent
#endif
    void leaveEvent(QEvent *e) override;       // leaveEvent 在 Qt 5/6 中都是 QEvent

private:
    QIcon normalIco, hoverIco;
};

#endif // HOVERBUTTON_H
