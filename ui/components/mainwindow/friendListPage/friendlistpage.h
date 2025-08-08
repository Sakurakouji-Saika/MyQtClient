#ifndef FRIENDLISTPAGE_H
#define FRIENDLISTPAGE_H

#include <QWidget>
#include "../../ui/components/contacts/FriendInfo.h"
#include "../../ui/components/contacts/friendlistwidget.h"
#include <qDebug>

namespace Ui {
class friendListPage;
}

class friendListPage : public QWidget
{
    Q_OBJECT

public:
    explicit friendListPage(QWidget *parent = nullptr);
    ~friendListPage();

private:

    FriendListWidget *m_friendListWidget; // 好友列表控件


    Ui::friendListPage *ui;
};

#endif // FRIENDLISTPAGE_H
