#ifndef HOVERBUTTON_H
#define HOVERBUTTON_H

#include <QObject>
#include <QPushButton>

class HoverButton : public QPushButton
{
    Q_OBJECT
public:
    explicit HoverButton(QWidget *parent = nullptr);
    void setNormalIcon(const QIcon&);
    void setHoverIcon(const QIcon&);

protected:
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;

private:
    QIcon normalIco, hoverIco;
};

#endif // HOVERBUTTON_H
