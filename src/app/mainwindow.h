#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSize>
#include <QMenu>
#include <QMainWindow>
#include <hoverbutton.h>
#include <QSystemTrayIcon>
#include <QGraphicsDropShadowEffect>

#include "../utils/clientsocket.h"
#include "../../ui/components/mainwindow/chatListPage/chatlistpage.h"
#include "../../ui/components/mainwindow/friendListPage/friendlistpage.h"
#include "../../ui/components/ProfilePage/profilepage_main.h"
#include "../../ui/components/chatListPage/chatlist_main.h"
#include "../../ui/components/FriendNotify/friendnotify_page.h"
#include "../../ui/components/addfrienddialog/addfrienddialog.h"

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
    void SetSocket(ClientSocket *tcpSocket, const QString &name);

private slots:

    void SltTrayIcoClicked(QSystemTrayIcon::ActivationReason reason);
    void SltTrayIconMenuClicked(QAction *action);

    void SltTcpReply(const quint8&type,const QJsonValue &dataVal);
    void SltTcpStatus(const quint8&staus);


    void SltQuitApp();
    void on_MinBtn_clicked();

    void on_searchBtn_clicked();

private:
    Ui::MainWindow *ui;

    QSystemTrayIcon *systemTrayIcon;
    chatListPage * chatList;            // 已经打开的聊天页面
    friendListPage * friendList;        // 好友列表页

    ProfilePage_Main *m_profile_main_page;      // 个人介绍主页
    QWidget *m_blankPage;                       // 空白页
    chatList_Main *m_chatlist_page;             // 聊天页面
    FriendNotify_Page *m_friendNotify;          // 同意添加好友通知页面
    addfrienddialog * m_addfriend = nullptr;              // 搜索添加好友页面

    // socket通信类
    ClientSocket    *m_tcpSocket;

    // 主动退出操作时不进行断线匹配
    bool            m_bQuit;

};

#endif // MAINWINDOW_H
