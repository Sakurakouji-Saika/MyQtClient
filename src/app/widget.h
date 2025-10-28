#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileSystemWatcher>
#include <HoverButton.h>
#include <QJsonObject>
#include <QJsonDocument>
#include "../../Src/Network/networkadapter.h"

#include "../../ui/components/Registration_Page/registration_page.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class LoginPage;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    void initProfilePicture();
    void loadStyleCloseBtn();
    void otherStyle();
    void updateLoginBtnStyle();
    void addWidget_IPInput();


public:
    Widget(NetworkAdapter *network,QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_setupBtn_clicked();

    void on_setupBtn_2_clicked();

    void on_config_Ok_Btn_clicked();

    void on_loginBtn_clicked();

    // 初始化网络
    void NetWorkInit(NetworkAdapter *network);

    // 初始化数据库
    void InitDataBaseMange();

    void on_SignalStatus(const quint8 &state);

    void on_SignalMessage(const quint8 &type,const QJsonValue &dataVal);

    void on_registerBtn_clicked();

private:
    Ui::LoginPage *ui;

    NetworkAdapter *m_network = nullptr;
    Registration_Page *m_registrationPage = nullptr;  //注册变量



};
#endif // WIDGET_H
