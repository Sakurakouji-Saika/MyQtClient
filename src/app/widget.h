#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileSystemWatcher>
#include <HoverButton.h>
#include <QJsonObject>
#include <QJsonDocument>
#include "../utils/clientsocket.h"

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
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_setupBtn_clicked();

    void on_setupBtn_2_clicked();

    void on_config_Ok_Btn_clicked();

    void on_loginBtn_clicked();


    void on_SignalStatus(const quint8 &state);

    void on_SignalMessage(const quint8 &type,const QJsonValue &dataVal);

private:
    Ui::LoginPage *ui;

    ClientSocket *m_clientSocket;


#ifdef QT_DEBUG
    QFileSystemWatcher  m_fileWatcher;  // <-- 成员变量
#endif

};
#endif // WIDGET_H
