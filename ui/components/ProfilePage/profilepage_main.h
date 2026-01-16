#ifndef PROFILEPAGE_MAIN_H
#define PROFILEPAGE_MAIN_H

#include <QWidget>
#include "../../src/DataBaseManage/model/FriendInfo.h"
#include "../../src/utils/utils.h"
#include "../../src/Network/Service/servicemanager.h"
#include "../../src/Network/Service/friendservice.h"



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
    void setNetWork(ServiceManager* _sm);

private slots:
    void on_btn_push_msg_clicked();

    void on_btn_clear_friend_clicked();

    void on_DeleteFriendSuccessSignals(qint64 delete_uid);

signals:
    void open_friend_chat_page(const FriendInfo &fi);

    void deleteFriendUid(qint64 friend_uid);


private:
    ServiceManager *m_sm;
    FriendService *m_fs;


    FriendInfo m_info;

    Ui::ProfilePage_Main *ui;
};

#endif // PROFILEPAGE_MAIN_H
