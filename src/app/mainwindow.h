#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMenu>
#include <QMainWindow>
#include <hoverbutton.h>
#include <QSystemTrayIcon>

#include "../utils/clientsocket.h"
#include "../../ui/components/mainwindow/chatListPage/chatlistpage.h"
#include "../../ui/components/mainwindow/friendListPage/friendlistpage.h"

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
    void on_contactsButton_clicked();

    void SltTrayIcoClicked(QSystemTrayIcon::ActivationReason reason);
    void SltTrayIconMenuClicked(QAction *action);

    void SltTcpReply(const quint8&type,const QJsonValue &dataVal);
    void SltTcpStatus(const quint8&staus);


    void SltQuitApp();
    void on_MinBtn_clicked();

private:
    Ui::MainWindow *ui;

    QSystemTrayIcon *systemTrayIcon;
    chatListPage * chatList;
    friendListPage * friendList;

    // socket通信类
    ClientSocket    *m_tcpSocket;

    // 主动退出操作时不进行断线匹配
    bool            m_bQuit;

};

#endif // MAINWINDOW_H
