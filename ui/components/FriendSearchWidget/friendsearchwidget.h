#ifndef FRIENDSEARCHWIDGET_H
#define FRIENDSEARCHWIDGET_H

#include <QWidget>

namespace Ui {
class FriendSearchWidget;
}

class FriendSearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FriendSearchWidget(QWidget *parent = nullptr);
    ~FriendSearchWidget();

private:
    Ui::FriendSearchWidget *ui;
};

#endif // FRIENDSEARCHWIDGET_H
