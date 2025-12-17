#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileSystemWatcher>
#include <HoverButton.h>
#include <QJsonObject>
#include <QJsonDocument>
#include "mainwindow.h"
#include "../Network/Service/authservice.h"

#include "../Network/Service/servicemanager.h"
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
    void InitDataBaseMange();
    void setNetwork(ServiceManager *_sm);

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    void on_setupBtn_clicked();
    void on_setupBtn_2_clicked();
    void on_config_Ok_Btn_clicked();
    void on_loginBtn_clicked();
    void on_registerBtn_clicked();

    void on_forgotPasswordBtn_clicked();

private:

    Ui::LoginPage *ui;

    Registration_Page *m_registrationPage = nullptr;  //注册变量
    ServiceManager *m_sm;

    MainWindow *m_mw = nullptr;
};
#endif // WIDGET_H
