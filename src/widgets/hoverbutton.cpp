// hoverbutton.cpp
#include "hoverbutton.h"

HoverButton::HoverButton(QWidget *parent)
    : QPushButton(parent)
{
}

void HoverButton::setNormalIcon(const QIcon &icon)
{
    normalIco = icon;
    setIcon(normalIco);
}

void HoverButton::setHoverIcon(const QIcon &icon)
{
    hoverIco = icon;
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void HoverButton::enterEvent(QEnterEvent *e)
#else
void HoverButton::enterEvent(QEvent *e)
#endif
{
    setIcon(hoverIco);
    QPushButton::enterEvent(e);
}

void HoverButton::leaveEvent(QEvent *e)
{
    setIcon(normalIco);
    QPushButton::leaveEvent(e);
}
