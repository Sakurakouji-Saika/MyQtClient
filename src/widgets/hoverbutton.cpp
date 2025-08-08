// hoverbutton.cpp
#include "hoverbutton.h"
#include <QEvent>

HoverButton::HoverButton(QWidget *parent) : QPushButton(parent) {}

void HoverButton::setNormalIcon(const QIcon &ico) {
    normalIco = ico;
    setIcon(normalIco);
}

void HoverButton::setHoverIcon(const QIcon &ico)  {
    hoverIco  = ico;
}

void HoverButton::enterEvent(QEvent *e) {
    if (!hoverIco.isNull())
        setIcon(hoverIco);
    QPushButton::enterEvent(e);
}

void HoverButton::leaveEvent(QEvent *e) {
    if (!normalIco.isNull())
        setIcon(normalIco);
    QPushButton::leaveEvent(e);
}
