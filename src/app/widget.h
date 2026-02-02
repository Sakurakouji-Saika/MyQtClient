#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileSystemWatcher>
#include "hoverbutton.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "mainwindow.h"
#include "../Network/Service/authservice.h"

#include "../Network/Service/servicemanager.h"
#include "../../ui/components/Registration_Page/registration_page.h"


QT_BEGIN_NAMESPACE


class IPLineEdit;
namespace Ui {
class LoginPage;
}
QT_END_NAMESPACE

class handlerregistry;

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
    void setNetwork();

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

    void LinkSignalList();

    void on_config_cancel_Btn_clicked();

private:

    Ui::LoginPage *ui;

    Registration_Page *m_registrationPage = nullptr;  //注册变量
    ServiceManager *m_sm;
    handlerregistry * hr;

    MainWindow *m_mw = nullptr;


    bool m_dragging = false;
    QPoint m_dragPosition;

    IPLineEdit *ip = nullptr;

    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};
#endif // WIDGET_H
