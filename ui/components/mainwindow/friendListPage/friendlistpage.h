#ifndef FRIENDLISTPAGE_H
#define FRIENDLISTPAGE_H

#include <QWidget>
#include "../../src/DataBaseManage/model/FriendInfo.h"
#include "../../ui/components/friendListPage/friendlistwidget.h"
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



signals:
    void signals_open_profile_page(const FriendInfo &fi);

private:

    FriendListWidget *m_friendListWidget; // 好友列表控件



    Ui::friendListPage *ui;

};

#endif // FRIENDLISTPAGE_H
