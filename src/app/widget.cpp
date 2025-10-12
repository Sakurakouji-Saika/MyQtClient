#pragma once

#include "widget.h"
#include "./ui_widget.h"

#include <QPainter>
#include <QBitmap>
#include <QPainterPath>

#include <QWidget>
#include <QFile>
#include <QFileSystemWatcher>
#include <QDebug>

#include <QMessageBox>


#include "iplineedit.h"
#include "../app/mainwindow.h"
#include "../utils/StyleLoader.h"
#include "../utils/comapi/unit.h"

// #define STYLE_SHEET_PATH "../styles/loginPage.css"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginPage)
{

    ui->setupUi(this);


    // 验证资源文件是否存在
    QString qssPath = ":/styles/loginPage.css";

    // 加载样式
    StyleLoader::loadWidgetStyle(this, qssPath);


    initProfilePicture();       // 初始化登录页面头像效果
    loadStyleCloseBtn();        // 这里是加载关闭窗口按钮图标
    otherStyle();               // 其他样式
    addWidget_IPInput();

    //m_tcpSocket
    m_clientSocket = new ClientSocket;
    connect(m_clientSocket,&ClientSocket::signalStatus,this,[this](const quint8 &state){
        // 这里写你槽函数的内容，比如调用成员函数
        on_SignalStatus(state);
    });


    connect(m_clientSocket,&ClientSocket::signalMessage,this,[this](const quint8 &type,const QJsonValue &dataVal){

        on_SignalMessage(type,dataVal);
    });

    m_clientSocket->CheckConnected();
}

Widget::~Widget()
{
    delete ui;
}


void Widget::initProfilePicture()
{
    // 1. 加载原始图片
    QPixmap src(":/picture/avatar/1.jpg");

    // 2. 缩放到目标大小（假设 85×85）
    const int size = 86;
    QPixmap scaled = src.scaled(size, size,
                                Qt::KeepAspectRatioByExpanding,
                                Qt::SmoothTransformation);

    // 3. 准备一个透明底的 QPixmap 来绘制圆形剪裁结果
    QPixmap result(size, size);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // 4. 用 QPainterPath 定义一个圆形区域，并裁剪
    QPainterPath path;
    path.addEllipse(0, 0, size, size);
    painter.setClipPath(path);

    // 5. 把缩放后的原图画上去
    painter.drawPixmap(0, 0, scaled);
    painter.end();

    // 6. 设置到 QLabel
    ui->profilePicture->setPixmap(result);
}



void Widget::loadStyleCloseBtn()
{
    auto btn = ui->closeBtn;
    auto btn2 = ui->closeBtn_2;

    QIcon icoNormal(":/svg/Close.svg");
    QIcon icoHover( ":/svg/CloseOff.svg");

    //登陆页面第一页 关闭按钮
    btn->setNormalIcon(icoNormal);
    btn->setHoverIcon(icoHover);
    btn->setIconSize(QSize(16,16));

    //登陆页面第二页 关闭按钮
    btn2->setNormalIcon(icoNormal);
    btn2->setHoverIcon(icoHover);
    btn2->setIconSize(QSize(16,16));
}

void Widget::otherStyle()
{
    // 每当用户编辑内容时，都重新检查一次
    connect(ui->userEdit, &QLineEdit::textChanged,
            this, &Widget::updateLoginBtnStyle);
    connect(ui->passwordEdit, &QLineEdit::textChanged,
            this, &Widget::updateLoginBtnStyle);



    // IP 输入框样式

}

void Widget::updateLoginBtnStyle()
{
    // 使用 trimmed() 避免全空格也算“有内容”
    bool hasUser     = !ui->userEdit->text().trimmed().isEmpty();
    bool hasPassword = !ui->passwordEdit->text().trimmed().isEmpty();

    if (hasUser && hasPassword) {
        ui->loginBtn->setStyleSheet(
            "QPushButton#loginBtn {"
            "    background-color: #0099FF;"
            "    color: #ffffff;"
            "}"
            "QPushButton#loginBtn:hover {"
            "    background-color: #008DEB;"  // hover 时加深色
            "}"
            );
        ui->loginBtn->setEnabled(true);
    } else {
        // 样式清空回到默认，且按钮禁用（如果需要）
        ui->loginBtn->setStyleSheet("");
        ui->loginBtn->setEnabled(false);
    }
}

void Widget::addWidget_IPInput()
{
    // 1) 先拿到 widget_5 上的 QVBoxLayout
    auto *lay = qobject_cast<QVBoxLayout*>(ui->widget_5->layout());

    // 2) 创建你的 IPLineEdit
    auto *ip = new IPLineEdit(this);
    ip->setObjectName("IP_Input");

    // 3) 找到 IP_Title_Label 在布局中的索引
    int idx = lay->indexOf(ui->IP_Title_Label);

    // 4) 把 IPLineEdit 插到它下面一行（所以是 idx+1）
    lay->insertWidget(idx + 1, ip);



}





void Widget::on_setupBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void Widget::on_setupBtn_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}





void Widget::on_config_Ok_Btn_clicked()
{
    IPLineEdit *ipWidget = this->findChild<IPLineEdit*>("IP_Input");
    if (ipWidget) {
        qDebug() << ipWidget->text();  // 成功输出 QString 类型的 IP 字符串
    } else {
        qDebug() << "IP_Input not found!";
    }
}


void Widget::on_loginBtn_clicked()
{



    // 添加按钮禁用防止重复点击
    ui->loginBtn->setEnabled(false);

    QString userName = ui->userEdit->text();
    QString passwd = ui->passwordEdit->text();

    QJsonObject json;
    json.insert("name", userName);
    json.insert("passwd", passwd);

    // 确保socket已初始化
    if (!m_clientSocket) {
        // 初始化处理...
        m_clientSocket = new ClientSocket;
    }

    // 异步发送
    m_clientSocket->SltSendMessage(0x11, json);



    // QString userName = ui->userEdit->text();
    // QString passwd = ui->passwordEdit->text();

    // QJsonObject json;
    // json.insert("name",userName);
    // json.insert("passwd",passwd);

    // m_clientSocket->SltSendMessage(0x11,json);


}

void Widget::on_SignalStatus(const quint8 &state)
{
    switch (state) {
    case LoginSuccess:  // 用户登陆成功
    {
        disconnect(m_clientSocket, &ClientSocket::signalStatus,
                   this, nullptr);

        disconnect(m_clientSocket, &ClientSocket::signalMessage,
                   this, nullptr);

        MainWindow *mainWindow = new MainWindow();
        mainWindow->show();
        mainWindow->SetSocket(m_clientSocket,ui->userEdit->text());
        this->hide();
        break;
    }
    case LoginPasswdError:  // 未注册
        QMessageBox::warning(this, "登录失败", "该用户未注册！");
        break;

    case LoginRepeat:  // 用户已在线
        QMessageBox::warning(this, "登录失败", "该用户已在线！");
        break;

    case ConnectedHost:  // 用户已在线
        qDebug() << "客户端成功访问到服务器IP地址为可用";
        break;

    default:  // 用户已在线
        QMessageBox::warning(this, "登录失败", "未知错误");
        break;
    }



}

void Widget::on_SignalMessage(const quint8 &type, const QJsonValue &dataVal)
{

}


void Widget::on_registerBtn_clicked()
{

    if (m_registrationPage== nullptr) {
        m_registrationPage = new Registration_Page();
        m_registrationPage->setAttribute(Qt::WA_DeleteOnClose);

        connect(m_registrationPage, &Registration_Page::destroyed, this, [this](){
            m_registrationPage = nullptr;
        });

    }
    m_registrationPage->show();
    m_registrationPage->raise();
    m_registrationPage->activateWindow();
}

