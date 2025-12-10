#include "registration_page.h"
#include "ui_registration_page.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <QCryptographicHash>

Registration_Page::Registration_Page(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Registration_Page)
{
    ui->setupUi(this);

    StyleLoader::loadWidgetStyle(this, ":/styles/registration_page.css");
}

Registration_Page::~Registration_Page()
{
    delete ui;
}

void Registration_Page::setNetwork(ServiceManager *_sm)
{
    m_sm = _sm;

    AuthService* auth = m_sm->auth();
    if (!auth) return;

    // 登录成功：显示信息并恢复登录按钮
    connect(auth, &AuthService::registrationSucceede, this, [this](qint64 userId){

        QMessageBox::information(this, QStringLiteral("注册成功"),
                                 QStringLiteral("注册成功,用户ID: %1 ").arg(userId));




    });

    // 登录失败：弹出错误并恢复登录按钮
    connect(auth, &AuthService::regostrationFailed, this, [this](const QString &reason){
        QMessageBox::warning(this, QStringLiteral("登录失败"), reason);
    });




}

void Registration_Page::on_pushButton_clicked()
{
    // 预处理：去除前后空白
    QString name = ui->RP_name->text().trimmed();
    QString password = ui->RP_pwd->text();
    QString password_ok = ui->RP_pwd_ok->text();
    QString email = ui->RP_email->text().trimmed();

    // 非空检查
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("输入错误"), tr("用户名不能为空"));
        ui->RP_name->setFocus();
        return;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, tr("输入错误"), tr("密码不能为空"));
        ui->RP_pwd->setFocus();
        return;
    }
    if (password_ok.isEmpty()) {
        QMessageBox::warning(this, tr("输入错误"), tr("请再次输入密码以确认"));
        ui->RP_pwd_ok->setFocus();
        return;
    }

    // 密码确认
    if (password != password_ok) {
        QMessageBox::warning(this, tr("输入错误"), tr("两次输入的密码不一致，请重新输入"));
        ui->RP_pwd_ok->clear();
        ui->RP_pwd_ok->setFocus();
        return;
    }

    // 邮箱格式简单校验
    QRegularExpression emailRe(R"((^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$))");
    if (email.isEmpty() || !emailRe.match(email).hasMatch()) {
        QMessageBox::warning(this, tr("输入错误"), tr("请输入有效的邮箱地址"));
        ui->RP_email->setFocus();
        return;
    }

    m_sm->auth()->registration(name,password,email);

}

