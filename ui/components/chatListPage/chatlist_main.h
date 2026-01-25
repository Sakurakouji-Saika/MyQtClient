#ifndef CHATLIST_MAIN_H
#define CHATLIST_MAIN_H

#include <QWidget>
#include "chat-widget/chatwidget.h"
#include "../../src/DataBaseManage/databasemanage.h"
#include "../../src/DataBaseManage/model/ChatRecord.h"
#include "../../src/utils/appconfig.h"

class ServiceManager;

namespace Ui {
class chatList_Main;
}

class chatList_Main : public QWidget
{
    Q_OBJECT

public:
    explicit chatList_Main(QWidget *parent = nullptr);
    ~chatList_Main();

    void openChatPage(const int _id);
    void addChatLeft(bool isMy,const QString avatar,const QString msg);
    void MsgALLClear();
    void SetNetWork(ServiceManager *_sm);
    int getUserID(){ return m_user_id;}


signals:
    void MY_SeedMsg(const ChatRecord &msg);

private slots:
    void on_btn_pushMsg_clicked();

private:
    int m_user_id;
    // QString m_avatar_url;
    QString m_user_name;

private:
    ChatWidget *chat;

    ServiceManager *m_sm;
    Ui::chatList_Main *ui;
};

#endif // CHATLIST_MAIN_H
