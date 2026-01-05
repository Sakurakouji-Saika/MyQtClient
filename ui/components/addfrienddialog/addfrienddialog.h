#ifndef ADDFRIENDDIALOG_H
#define ADDFRIENDDIALOG_H

#include <QWidget>
#include <QJsonObject>
#include "../../src/utils/styleLoader.h"
#include "../../src/utils/utils.h"
#include "../../src/DataBaseManage/databasemanage.h"

class ServiceManager;
class FriendService;  // 前向声明

namespace Ui {
class addfrienddialog;
}

class addfrienddialog : public QWidget
{
    Q_OBJECT

public:
    explicit addfrienddialog(QWidget *parent = nullptr);
    ~addfrienddialog();

    void setNetWork(ServiceManager *_sm);
    void on_return_addFriend_Info(const QJsonValue &info);

private slots:
    void on_add_friend_btn_clicked();
    void on_AF_use_2_btn_clicked();
    void onSearchFriendSuccess(qint64 uid, QString userName, QString nickname, qint64 avatar_file_id, QString avatar);  // 新增槽函数

private:
    Ui::addfrienddialog *ui;
    ServiceManager *m_sm;
    FriendService *m_fs;
    bool isConnected;
};

#endif // ADDFRIENDDIALOG_H
