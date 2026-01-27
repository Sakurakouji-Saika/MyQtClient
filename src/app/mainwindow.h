#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSize>
#include <QMenu>
#include <QTimer>
#include <QMouseEvent>
#include <QMainWindow>
#include <QPropertyAnimation>
#include <hoverbutton.h>
#include <QSystemTrayIcon>
#include <QGraphicsDropShadowEffect>

#include "../../src/DataBaseManage/databasemanage.h"
#include "../../ui/components/mainwindow/chatListPage/chatlistpage.h"
#include "../../ui/components/mainwindow/friendListPage/friendlistpage.h"
#include "../../ui/components/mainwindow/friendListPage/friendlistpage.h"
#include "../../ui/components/ProfilePage/profilepage_main.h"
#include "../../ui/components/chatListPage/chatlist_main.h"
#include "../../ui/components/FriendNotify/friendnotify_page.h"
#include "../../ui/components/addfrienddialog/addfrienddialog.h"
#include "../../ui/components/Change_Avatar_Page/change_avatar_page.h"
#include "../../src/DataBaseManage/model/ChatRecord.h"
#include "../../src/Network/Service/servicemanager.h"



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
    void initAvatarManager();

    void loadStyleCloseBtn();
    void Open_Edit_Avatar_Page();

    void setNetWork(ServiceManager *_sm);


private slots:

    void SltTrayIcoClicked(QSystemTrayIcon::ActivationReason reason);
    void SltTrayIconMenuClicked(QAction *action);
    void SltQuitApp();
    void on_MinBtn_clicked();
    void on_searchBtn_clicked();




    void on_Friend_OnlineSignal(qint64 friend_uid,int state);
    void on_UpdateAvatarSignal(qint64 uid,qint64 file_id);
    void on_RemovedByFriendSignal(qint64 uid);
    void on_friendAddedSignal(qint64 uid, QString avatarName, qint64 avatar_file_id, QString nickname, int status, QString username);
    void on_ReceiveNewMessageSignals(qint64 file_id, QString msgContent, int msgType, qint64 send_at, qint64 sender_id, qint64 msgId, qint64 receiver_id);
    void on_ReceiveNewMsgSuccessSignals(qint64 msgID);


    void on_closeBtn_clicked();


    void on_MaxBtn_clicked();

    void on_fileManagerButton_clicked();

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

    // 移动窗口相关函数
    bool m_dragging = false;
    QPoint m_dragPosition;

    QRect m_restoreGeometry;               // 保存恢复到正常状态时的 geometry
    QPropertyAnimation *m_restoreAnimation = nullptr;


    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

};



#endif // MAINWINDOW_H
