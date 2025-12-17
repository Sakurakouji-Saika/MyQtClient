#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSize>
#include <QMenu>
#include <QTimer>
#include <QMouseEvent>
#include <QMainWindow>
#include <hoverbutton.h>
#include <QSystemTrayIcon>
#include <QGraphicsDropShadowEffect>

#include "../../Src/DataBaseManage/databasemanage.h"
#include "../../ui/components/mainwindow/chatListPage/chatlistpage.h"
#include "../../ui/components/mainwindow/friendListPage/friendlistpage.h"
#include "../../ui/components/ProfilePage/profilepage_main.h"
#include "../../ui/components/chatListPage/chatlist_main.h"
#include "../../ui/components/FriendNotify/friendnotify_page.h"
#include "../../ui/components/addfrienddialog/addfrienddialog.h"
#include "../../ui/components/Change_Avatar_Page/change_avatar_page.h"
#include "../../Src/DataBaseManage/model/ChatRecord.h"
#include "../../Src/Network/Service/servicemanager.h"




namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void InitSysTrayIcon();
    void initProfilePicture();
    void initStackedWidgetPages();
    void loadStyleCloseBtn();
    void Open_Edit_Avatar_Page();
    // 测试下载头像
    void DownloadAvatarTest();

    void setNetWork(ServiceManager *_sm);


private slots:

    void SltTrayIcoClicked(QSystemTrayIcon::ActivationReason reason);
    void SltTrayIconMenuClicked(QAction *action);
    void SltQuitApp();
    void on_MinBtn_clicked();
    void on_searchBtn_clicked();



private:
    Ui::MainWindow *ui;

    QSystemTrayIcon *systemTrayIcon;
    chatListPage * chatList;                    // 已经打开的聊天页面
    friendListPage * friendList;                // 好友列表页
    ProfilePage_Main *m_profile_main_page;      // 个人介绍主页
    QWidget *m_blankPage;                       // 空白页
    chatList_Main *m_chatlist_page;             // 聊天页面
    FriendNotify_Page *m_friendNotify;          // 同意添加好友通知页面
    addfrienddialog * m_addfriend = nullptr;    // 搜索添加好友页面
    Change_Avatar_Page * m_CAvatarPG = nullptr; // 修改头像页面

    ServiceManager * m_sm = nullptr;

    // 主动退出操作时不进行断线匹配
    bool            m_bQuit;


    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // MAINWINDOW_H
