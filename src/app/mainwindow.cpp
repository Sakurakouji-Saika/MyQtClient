#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../utils/StyleLoader.h"
#include <QPainter>
#include <QPainterPath>
#include <QButtonGroup>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // 设计两页分栏
    QList<int> sizes;
    sizes.append(250); // 第一个值 250 对应 splitter 的第一个子控件的宽度
    sizes.append(this->width() - 250);  // 第二个值 的宽度，占满剩下的宽度
    ui->splitter->setSizes(sizes);


    // 验证资源文件是否存在
    QString qssPath = ":/styles/mainwindow.css";
    // 加载样式
    StyleLoader::loadWidgetStyle(this, qssPath);

    initProfilePicture();

    //初始化 stack 页面(聊天和好友页)
    initStackedWidgetPages();

    //设置关闭按钮样式
    loadStyleCloseBtn();


    // 去掉标题栏
    // this->setWindowFlags(Qt::FramelessWindowHint);//去掉QT自带的状态栏
    // this->setAttribute(Qt::WA_TranslucentBackground);//背景透明

    m_bQuit = false;


    InitSysTrayIcon();
}

MainWindow::~MainWindow()
{
    delete ui;

    delete chatList;
    delete friendList;
}

void MainWindow::initProfilePicture()
{
    // 1. 加载原始图片
    QPixmap src(":/picture/avatar/1.jpg");

    // 2. 缩放到目标大小（假设 85×85）
    const int size = 40;
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
    ui->avatar->setPixmap(result);
}

void MainWindow::initStackedWidgetPages()
{
    chatList = new chatListPage();
    friendList = new friendListPage();

    QButtonGroup *btnGroup = new QButtonGroup(this);

    // 为按钮分配 ID
    btnGroup->addButton(ui->chatButton, 0);
    btnGroup->addButton(ui->contactsButton, 1);


    // 使用 QOverload<int>::of 正确连接信号
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 12)
    connect(btnGroup, QOverload<int>::of(&QButtonGroup::idClicked),
            ui->friendSelectorStack, &QStackedWidget::setCurrentIndex);
#else
    connect(btnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            ui->friendSelectorStack, &QStackedWidget::setCurrentIndex);
#endif


    ui->friendSelectorStack->addWidget(chatList);
    ui->friendSelectorStack->addWidget(friendList);
    ui->friendSelectorStack->setCurrentIndex(0);
}

void MainWindow::loadStyleCloseBtn()
{

    auto btn = ui->closeBtn;


    QIcon icoNormal(":/svg/Close.svg");
    QIcon icoHover( ":/svg/CloseOff.svg");

    //登陆页面第一页 关闭按钮
    btn->setNormalIcon(icoNormal);
    btn->setHoverIcon(icoHover);
    btn->setIconSize(QSize(16,16));


}


void MainWindow::on_contactsButton_clicked()
{

}





/**
 * @brief MainWindow::InitSysTrayIcon
 * 托盘菜单
 */
void MainWindow::InitSysTrayIcon()
{
    systemTrayIcon = new QSystemTrayIcon(this);
    systemTrayIcon->setIcon(QIcon("://picture/favicon.ico"));

    QMenu *m_trayMenu = new QMenu(this);
    m_trayMenu->addAction("我在线上");
    m_trayMenu->addAction("离线");
    m_trayMenu->addSeparator();
    m_trayMenu->addAction("显示主面板");
    m_trayMenu->addSeparator();
    m_trayMenu->addAction("退出");

    systemTrayIcon->setContextMenu(m_trayMenu);
    systemTrayIcon->show();

    connect(systemTrayIcon, &QSystemTrayIcon::activated,
            this, &MainWindow::SltTrayIcoClicked);
    connect(m_trayMenu,&QMenu::triggered,this,&MainWindow::SltTrayIconMenuClicked);

}



void MainWindow::SltTrayIcoClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
        case QSystemTrayIcon::Trigger:{

        }
        break;

        case QSystemTrayIcon::DoubleClick:{
            if(!this->isVisible()){
                this->show();
            }
        }
        break;

        default:

            break;
    }
}




void MainWindow::SltTrayIconMenuClicked(QAction *action)
{
    if( action->text() == "退出"){

        this->hide();
        QTimer::singleShot(500, this, &MainWindow::SltQuitApp);
    }else if("显示主面板" == action->text()){
        this->show();
    }else if(!QString::compare("我在线上",action->text()))
    {
        m_tcpSocket->CheckConnected();
    }else if(!QString::compare("离线",action->text())){
        m_tcpSocket->CloseConnected();
    }
}



void MainWindow::SetSocket(ClientSocket *tcpSocket, const QString &name)
{
    if(nullptr!=tcpSocket){
        m_tcpSocket = tcpSocket;

        connect(m_tcpSocket,&ClientSocket::signalMessage,this,[this](const quint8 &type, const QJsonValue &dataVal){
            SltTcpReply(type,dataVal);
        });

        connect(m_tcpSocket,&ClientSocket::signalStatus,this,[this](const quint8 &state){
            SltTcpStatus(state);
        });
    }
}


void MainWindow::on_MinBtn_clicked()
{
    this->hide();
}



void MainWindow::SltQuitApp()
{
    qApp->quit();
}



void MainWindow::SltTcpReply(const quint8 &type, const QJsonValue &dataVal)
{

}



void MainWindow::SltTcpStatus(const quint8 &staus)
{

}

