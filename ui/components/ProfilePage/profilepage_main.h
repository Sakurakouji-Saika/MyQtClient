#ifndef PROFILEPAGE_MAIN_H
#define PROFILEPAGE_MAIN_H

#include <QWidget>
#include "../../src/DataBaseManage/model/FriendInfo.h"
#include "../../src/utils/utils.h"

namespace Ui {
class ProfilePage_Main;
}

class ProfilePage_Main : public QWidget
{
    Q_OBJECT

public:
    explicit ProfilePage_Main(QWidget *parent = nullptr);
    ~ProfilePage_Main();
    void addInfo(FriendInfo info);

private slots:
    void on_btn_push_msg_clicked();

signals:
    void open_friend_chat_page(const FriendInfo &fi);


private:
    FriendInfo m_info;

    Ui::ProfilePage_Main *ui;
};

#endif // PROFILEPAGE_MAIN_H
