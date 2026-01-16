#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../utils/styleLoader.h"
#include "../DataBaseManage/databasemanage.h"
#include "../../ui/components/mainwindow/chatListPage/recent_data.h"
#include <QPainter>
#include <QPainterPath>
#include <QButtonGroup>
#include <QFileInfo>

#include <QThread>
#include <QJsonArray>

#include "../Network/Service/avatarservice.h"
#include "../widgets/avatar/avatarmanager.h"
#include "../DataBaseManage/ViewModel/FriendAvatarDTO.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 过滤器注册
    ui->avatar->installEventFilter(this);

    // 设计两页分栏
    QList<int> sizes;
    sizes.append(250); // 第一个值 250 对应 splitter 的第一个子控件的宽度
    sizes.append(this->width() - 250);  // 第二个值 的宽度，占满剩下的宽度
    ui->splitter->setSizes(sizes);

    // 验证资源文件是否存在
    QString qssPath = ":/styles/mainwindow.css";
    // 加载样式
    StyleLoader::loadWidgetStyle(this, qssPath);


    // 初始化头像管理
    initAvatarManager();

    //
    initProfilePicture();

    //初始化 stack 页面(聊天和好友页)
    initStackedWidgetPages();

    //设置关闭按钮样式
    loadStyleCloseBtn();

    m_bQuit = false;

    InitSysTrayIcon();

    //``````````````````````````````````````````````````````````
    m_blankPage = new QWidget(this);
    ui->chatDetailStack->addWidget(m_blankPage); // index 0

    m_profile_main_page = new ProfilePage_Main(this);
    ui->chatDetailStack->addWidget(m_profile_main_page); // index 1

    m_chatlist_page = new chatList_Main(this);
    ui->chatDetailStack->addWidget(m_chatlist_page); // index 2

    m_friendNotify = new FriendNotify_Page(this);



    ui->chatDetailStack->addWidget(m_friendNotify); // index 3

    // 初始显示空白页
    ui->chatDetailStack->setCurrentIndex(0);

    connect(friendList,&friendListPage::signals_open_profile_page,[this](const FriendInfo &fi){
        ui->chatDetailStack->setCurrentIndex(1);
        m_profile_main_page->addInfo(fi);

        ui->centralwidget->setStyleSheet("#centralwidget { background-color: white; }");
    });

    connect(m_profile_main_page,&ProfilePage_Main::open_friend_chat_page,[this](const FriendInfo &fi){
        m_chatlist_page->MsgALLClear();

        m_chatlist_page->openChatPage(fi.friendId);

        ui->chatDetailStack->setCurrentIndex(2);
        ui->centralwidget->setStyleSheet("#centralwidget { background-color: #F2F2F2; }");

    });

    connect(chatList,&chatListPage::openChatPage,[this](const int &user_id){
        m_chatlist_page->MsgALLClear();

        m_chatlist_page->openChatPage(user_id);


        ui->chatDetailStack->setCurrentIndex(2);
        ui->centralwidget->setStyleSheet("#centralwidget { background-color: #F2F2F2; }");
    });


    connect(m_chatlist_page, &chatList_Main::MY_SeedMsg, this, [this](const ChatRecord &_CR){

        int peerId = _CR.toId;
        Recent_Data r;
        r.user_id = peerId;
        r.msg = _CR.content;
        r.msg_time = QDateTime::fromSecsSinceEpoch(_CR.timestamp);
        r.timestamp = _CR.timestamp;
        {
            r.avatarPath = AppConfig::instance().imagesDirectory() + QDir::separator() + AvatarManager::instance().avatarUrl(r.user_id);
        }

        qDebug() << "connect(m_chatlist_page, &chatList_Main::MY_SeedMsg::" <<  r.avatarPath;

        r.userName = DataBaseManage::instance()->getDisplayNameByFriendId(peerId);
        r.UnreadCount = 0;

        chatList->receiveMessage(r);
    });


    connect(m_profile_main_page,&ProfilePage_Main::deleteFriendUid,this,[this](){
        friendList->ReloadData();
    });

    connect(m_friendNotify,&FriendNotify_Page::updateFriendList,this,[this](){
        friendList->ReloadData();
    });
}

MainWindow::~MainWindow()
{
    delete ui;

    delete chatList;
    delete friendList;
}

void MainWindow::initProfilePicture()
{
    ui->avatar->setAvatar(AppConfig::instance().getUserID(),40);
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

void MainWindow::initAvatarManager()
{
    QList<FriendAvatar> avatarData;

    DataBaseManage::instance()->GetUserAvatarData(avatarData);

    for(auto &t : avatarData){
        AvatarManager::instance().updateAvatar(t.uid,t.fileName);
    }
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
        // m_tcpSocket->CheckConnected();
    }else if(!QString::compare("离线",action->text())){
        // m_tcpSocket->CloseConnected();
    }
}








void MainWindow::Open_Edit_Avatar_Page()
{
    if(m_CAvatarPG == nullptr){
        m_CAvatarPG = new Change_Avatar_Page();
        m_CAvatarPG->setAttribute(Qt::WA_DeleteOnClose);  // 关闭时自动 delete

        // 当窗口被 delete 时，自动把指针置空，避免悬空指针
        connect(m_CAvatarPG,&Change_Avatar_Page::destroyed,this,[this]{
            m_CAvatarPG = nullptr;
        });

        connect(m_CAvatarPG,&Change_Avatar_Page::avatarUploaded,this,[=](QString &avatarPath){

            qDebug() << avatarPath;

            m_sm->avatar()->UpoadLoadAvatarStart(avatarPath);
        });


        // 去掉最大化按钮
        m_CAvatarPG->setWindowFlags(
            Qt::Window
            | Qt::WindowTitleHint
            | Qt::WindowSystemMenuHint
            | Qt::WindowMinimizeButtonHint
            | Qt::WindowCloseButtonHint
            );


        m_CAvatarPG->show();
    }else{
        // 已存在就复用：如果被最小化则还原，若被隐藏则显示，最后置顶并激活
        if (m_CAvatarPG->isMinimized())
            m_CAvatarPG->showNormal();

        m_CAvatarPG->show();            // 确保可见
        m_CAvatarPG->raise();           // 置于顶层
        m_CAvatarPG->activateWindow();  // 激活窗口（获取焦点）
    }


}

void MainWindow::setNetWork(ServiceManager *_sm)
{
    m_sm = _sm;

    m_friendNotify->setNetWork(m_sm);
    m_profile_main_page->setNetWork(m_sm);

}


void MainWindow::on_MinBtn_clicked()
{
    this->hide();
}



void MainWindow::SltQuitApp()
{
    qApp->quit();
}





void MainWindow::on_searchBtn_clicked()
{

    QMenu menu(this); // 栈对象，不会泄漏



    QAction *act1 = menu.addAction("添加好友");
    act1->setIcon(QIcon("://svg/add_group_24.svg"));

    QAction *act2 = menu.addAction("好友通知");
    act2->setIcon(QIcon("://svg/Notification.svg"));


    // 关闭系统阴影并去掉框架（使用 Popup 类型）
    menu.setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    // 允许透明背景（有助于边角过渡，虽然我们用 mask 裁形）
    menu.setAttribute(Qt::WA_TranslucentBackground);


    // 把图标尺寸设小一点（避免图标区域过大）
    // menu.setIcon(QSize(16, 16)); // 或 18/20，根据需要调整

    menu.setStyleSheet(R"(
    QMenu {
        background-color: white;
        border: 1px solid #cfcfcf;
        border-radius: 8px;
        padding: 6px;
    }
    /* item padding: top right bottom left —— 把左边 padding 调小 */
    QMenu::item {
        padding: 6px 12px 6px 6px;  /* 原来是 6px 20px -> 右边缩小到 12，左边缩到 6 */
        margin: 2px 0;
        spacing: 4px;               /* 有些 style 支持 spacing */
    }
    /* 图标到文字的额外间距（视 style 支持情况而定）*/
    QMenu::icon {
        padding: 0px 15px 0px 0px;   /* 上 右 下 左 —— 右边为 icon 到文字的距离，设为 6 */
        margin-left: 0px;           /* 清除可能的左侧预留 */
    }
    QMenu::item:selected {
        background-color: #e6e6e6;
        border-radius: 6px;
    }
    )");

    // 连接单个 QAction
    connect(act1, &QAction::triggered, this, [this](){
        qDebug() << "选项一被点击了";

        if (!m_addfriend) {
            m_addfriend = new addfrienddialog();
            m_addfriend->setNetWork(m_sm);

            m_addfriend->setAttribute(Qt::WA_DeleteOnClose);

            connect(m_addfriend, &QObject::destroyed, this, [this](){
                m_addfriend = nullptr;
            });



        }

        m_addfriend->show();
        m_addfriend->raise();
        m_addfriend->activateWindow();





    });
    connect(act2, &QAction::triggered, this, [this](){
        qDebug() << "选项二被点击了";

        // m_friendNotify->setNetWork(m_sm);
        m_friendNotify->test();

        ui->chatDetailStack->setCurrentIndex(3);
        ui->centralwidget->setStyleSheet("#centralwidget { background-color: #F2F2F2; }");


        // 测试：模拟收到新消息
        Recent_Data testData(
            ":/picture/avatar/11.jpg",
            "我喜欢你",
            1111,
            "薇婷",
            QDateTime::currentDateTime(),
            QDateTime::currentDateTime().toMSecsSinceEpoch(),
            3
            );

        chatList->receiveMessage(testData);

    });




    // 显示菜单
    QPoint pos = ui->searchBtn->mapToGlobal(QPoint(0, ui->searchBtn->height() + 5));
    menu.exec(pos);

    ui->searchBtn->setDown(false);   // 复位“按下”状态
    ui->searchBtn->setChecked(false);
    ui->searchBtn->update();

}



bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // 过滤器 处理关于点击主页面头像 弹出 设置头像页面
    if (watched == ui->avatar && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            Open_Edit_Avatar_Page();
            return true; // 事件已处理（仅左键）
        }
        // 不是左键则不处理，交给父类/其它处理器
    }
    return QMainWindow::eventFilter(watched, event);
}

