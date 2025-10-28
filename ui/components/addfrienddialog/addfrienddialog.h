#ifndef ADDFRIENDDIALOG_H
#define ADDFRIENDDIALOG_H

#include <QWidget>
#include <QJsonObject>

#include "../../src/utils/StyleLoader.h"
#include "../../Src/utils/utils.h"



#include "../../Src/Network/networkadapter.h"
#include "../../Src/DataBaseManage/databasemanage.h"



namespace Ui {
class addfrienddialog;
}

class addfrienddialog : public QWidget
{
    Q_OBJECT

public:
    explicit addfrienddialog(QWidget *parent = nullptr);
    ~addfrienddialog();

    void SetNetwork(NetworkAdapter *_net);


    void on_return_addFriend_Info(const QJsonValue &info);

private slots:
    void on_add_friend_btn_clicked();


    void on_AF_use_2_btn_clicked();

private:

    NetworkAdapter *m_net;

    Ui::addfrienddialog *ui;

};

#endif // ADDFRIENDDIALOG_H
