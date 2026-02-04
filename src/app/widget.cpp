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
#include <QJsonArray>

#include <QMessageBox>


#include "iplineedit.h"
#include "../utils/styleLoader.h"


#include "../app/mainwindow.h"
#include "../DataBaseManage/model/FriendsResponse.h"

#include "../utils/utils.h"

#include "../Network/Service/avatarservice.h"
#include "../Network/Handlers/handlerregistry.h"

#include "../widgets/hoverbutton.h"

#include "../widgets/avatar/avatarmanager.h"
#include "../Network/Service/friendservice.h"
#include <QWKWidgets/widgetwindowagent.h>
#include <QGuiApplication>
#include <QWindow>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginPage)
{
    ui->setupUi(this);


    auto agent = new QWK::WidgetWindowAgent(this);
    agent->setup(this);



    // 验证资源文件是否存在
    QString qssPath = ":/styles/loginPage.css";

    // 加载样式
    StyleLoader::loadWidgetStyle(this, qssPath);

    initProfilePicture();       // 初始化登录页面头像效果
    loadStyleCloseBtn();        // 这里是加载关闭窗口按钮图标
    otherStyle();               // 其他样式
    addWidget_IPInput();

    ui->widget->installEventFilter(this);
    ui->widget_4->installEventFilter(this);

    ui->Msg_Port_Line->setText(QString::number(AppConfig::instance().getPort()));
    ui->File_Prot_Line->setText(QString::number(AppConfig::instance().getFilePort()));

    setNetwork();
}

Widget::~Widget()
{
    delete ui;
}


void Widget::initProfilePicture()
{
    // 1. 加载原始图片
    QPixmap src(":/picture/avatar/7.jpg");

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
    HoverButton *btn = ui->closeBtn;
    HoverButton *btn2 = ui->closeBtn_2;

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
    ip = new IPLineEdit(this);
    ip->setObjectName("IP_Input");

    // 3) 找到 IP_Title_Label 在布局中的索引
    int idx = lay->indexOf(ui->IP_Title_Label);

    // 4) 把 IPLineEdit 插到它下面一行（所以是 idx+1）
    lay->insertWidget(idx + 1, ip);
    ip->setIP(AppConfig::instance().getHost());

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

    AppConfig::instance().setHost(ipWidget->text());
    AppConfig::instance().setPort(ui->Msg_Port_Line->text().toLongLong());
    AppConfig::instance().setFilePort(ui->File_Prot_Line->text().toLongLong());


    m_sm->stop();
    m_sm->start();

    hr->setServiceManager(m_sm);
    hr->registerAll();

    LinkSignalList();



    ui->stackedWidget->setCurrentIndex(0);
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

    std::optional<QPair<bool, QString>> temp_msg;

    m_sm->auth()->login(userName,passwd);

}



void Widget::InitDataBaseMange()
{
    int accountId = AppConfig::instance().getUserID();

    QString basePath = AppConfig::instance().databaseDirectory();

    // QString basePath =  "C:\\Users\\Moe\\Desktop\\MyClient\\datz`a\\";

    if (!DataBaseManage::instance()->init(accountId, basePath)) {
        qWarning() << "初始化数据库失败";
        return ;
    }
}

void Widget::setNetwork()
{

    m_sm = new ServiceManager();

    m_sm->init();
    m_sm->start();

    hr = new handlerregistry();
    hr->setServiceManager(m_sm);
    hr->registerAll();

    LinkSignalList();


}

void Widget::on_registerBtn_clicked()
{

    if (m_registrationPage== nullptr) {
        m_registrationPage = new Registration_Page();
        m_registrationPage->setAttribute(Qt::WA_DeleteOnClose);
        m_registrationPage->setNetwork(m_sm);

        connect(m_registrationPage, &Registration_Page::destroyed, this, [this](){
            m_registrationPage = nullptr;
        });

    }
    m_registrationPage->show();
    m_registrationPage->raise();
    m_registrationPage->activateWindow();
}


void Widget::on_forgotPasswordBtn_clicked()
{

    // 这里 是测试 一些接口忘记改了，对应的按钮是忘记密码的按钮。
    // 获取登录头像信息
    m_sm->avatar()->RequestAvatarInfoByUserID(1);


    // 获取登录头像
    m_sm->avatar()->requestAvatarByFileId("1");

}

void Widget::LinkSignalList()
{
    AuthService* auth = m_sm->auth();
    AvatarService * m_avatarService = m_sm->avatar();

    if (!auth) return;

    // 登录成功：显示信息并恢复登录按钮
    connect(auth, &AuthService::loginSucceeded, this, [this](const QJsonObject &resp){
        ui->loginBtn->setEnabled(false);

        QString idStr = resp.value("id").toString();
        qint64 uid = idStr.toLongLong();

        // 服务端字段
        qint64 avatar_file_id = resp.value("avatar_file_id").toVariant().toLongLong();

        if(avatar_file_id == 0){
            avatar_file_id = -1;
        }

        QString nickname = resp.value("nickname").toString();   // 服务端 nickname
        QString username = resp.value("username").toString();   // 服务端 username
        QString email = resp.value("email").toString();
        qint64 createdAt = resp.value("created_at").toString().toLongLong();
        qint64 updatedAt = resp.value("updated_at").toString().toLongLong();
        qint64 lastSeen = resp.value("last_seen").toString().toLongLong();
        QString avatar = resp.value("avatar").toString();


        qDebug() << "登录用户头像文件名：：" << avatar;

        AppConfig::instance().setUserID(uid);

        InitDataBaseMange();

        // 调用 upsertFriend（用服务端的 username/nickname/email 等字段）
        bool ok = DataBaseManage::instance()->upsertFriend(
            uid,
            username,
            nickname,
            email,
            avatar_file_id,
            avatar,
            1,  //登录状态
            createdAt,
            updatedAt
            );

        if (!ok) {
            qDebug() << "upsterFriend 更新数据失败" << uid;
        }



        // 其余 UI 流程
        QMessageBox::information(this, QStringLiteral("登录成功"),
                                 QStringLiteral("用户 %1 登录成功").arg(uid));



        m_sm->avatar()->RequestAvatarInfoByUserID(uid);

        // 获取好友列表
        m_sm->auth()->GetMyFriends(uid,5000);


    });

    // 动态加载头像
    connect(m_avatarService,&AvatarService::loadAvatarDynamicallySignals, this,
            [this](const qint64 user_id, const qint64 file_id, const QString fileName) {

                // 查看本地是否存在文件
                QString localFileName = AppConfig::instance().imagesDirectory() + QDir::separator() + fileName;
                QFileInfo fileInfo(localFileName);
                bool isFileExist = fileInfo.exists() && fileInfo.isFile();

                if(isFileExist){
                    AvatarManager::instance().updateAvatar(user_id, fileName);
                }else{

                    qDebug() << "loadAvatarDynamicallySignals::首次动态加载头像,需要下载文件:: " << fileName;
                    m_sm->avatar()->requestAvatarByFileId(QString::number(file_id),true);
                }


    });


    // 为登录用户设置头像
    // 1.如果本地数据库存在头像路径，就判断返回数据 和 本地数据的头像数据是否一致，如果一直就不下载头像文件,如果不一致就下载头像文件.
    // 2.如果本地数据库不存在头像路径，就插入到本地数据库中，然后下载头像文件。
    connect(m_avatarService, &AvatarService::avatarNicknameFetched, this,
            [this](const qint64 user_id, const qint64 file_id, const QString fileName) {

                // 查看本地是否存在文件
                QString localFileName = AppConfig::instance().imagesDirectory() + QDir::separator() + fileName;
                QFileInfo fileInfo(localFileName);
                bool isFileExist = fileInfo.exists() && fileInfo.isFile();


                std::optional<FriendInfo> info = DataBaseManage::instance()->GetFriendAvatarById(user_id);

                // 决定是否需要下载的逻辑：
                // 1. 如果文件不存在，肯定需要下载
                // 2. 如果数据库中没有记录，需要下载
                // 3. 如果有记录但信息不匹配，也需要下载

                bool needDownload = !isFileExist ||
                                    !info ||
                                    (info && (info->avatarFileId.toLongLong() != file_id ||
                                              info->avatar != fileName));

                if (needDownload) {
                    qDebug() << "需要下载头像:: user_id:" << user_id << ", file_id:" << file_id << ", fileName:" << fileName << ", 文件存在:" << isFileExist << ", 数据库有记录:" << (info ? "是" : "否");
                    m_sm->avatar()->requestAvatarByFileId(QString::number(file_id));

                } else {
                    qDebug() << "头像已存在且信息匹配，无需下载";
                    AvatarManager::instance().updateAvatar(user_id, fileName);
                }

            });


    connect(m_avatarService,&AvatarService::avatarNicknameFetchFailed,this,[this](QString error){

    });



    // 登录失败：弹出错误并恢复登录按钮
    connect(auth, &AuthService::loginFailed, this, [this](const QString &reason){
        ui->loginBtn->setEnabled(true);
        QMessageBox::warning(this, QStringLiteral("登录失败"), reason);
    });


    connect(m_sm->friendApi(),&FriendService::AddFriendSuccessSignals,this,[this](){
        QMessageBox::information(this, QStringLiteral("添加好友"),
                                 QStringLiteral("申请成功！"));

    });

    connect(m_sm->friendApi(),&FriendService::AddFriendErrorSignals,this,[this](QString error){
        QMessageBox::information(this, QStringLiteral("添加好友"),
                                 QStringLiteral("提示： %1").arg(error));

    });


    // 绑定获取好友列表
    connect(auth, &AuthService::GetMyFriendsSucceeded, this, [this](const QJsonObject &resp){
        qDebug() << "widget::app::setNetwork::&AuthService::GetMyFriendsSucceeded::resp" << resp << "\n\n";

        QString err;
        FriendsResponse fr = parseFriendsResponse(resp);

        bool ok = DataBaseManage::instance()->saveFriendListToDb(fr);

        // 开始下载好友头像
        if(ok){
            for(int i=0;i< fr.friends.size();i++){
                // 理论上这里还要读取本地客户端中关于存储头像的目录是否存在这个头像文件，存在就不用下载了
                if(!fileExistsInDir(AppConfig::instance().imagesDirectory(),fr.friends.at(i).avatarPath) && fr.friends.at(i).avatar_file_id !=-1){
                    qDebug() << "触发下载头像:: " << i << "\t fr.friends.at(i).avatarPath::" << fr.friends.at(i).avatarPath;
                    m_sm->avatar()->requestAvatarByFileId(QString::number(fr.friends.at(i).avatar_file_id));
                }
            }
        }



        // 获取离线消息
        qint64 userId = AppConfig::instance().getUserID();
        m_sm->auth()->GetOfflineMessage(userId,5000);

    });


    connect(auth,&AuthService::GetOfflineMessageSucceeded,this,[this](const QJsonObject &resp){
        qDebug() << "auth,&AuthService::GetOfflineMessageSucceeded::resp" << resp;

        QJsonArray list = resp.value("data").toArray();
        for(const QJsonValue &val : list){
            if (val.isObject()) {
                QJsonObject obj = val.toObject();
                qint64 ID = obj.value("ID").toInteger();
                qint64 Type = obj.value("Type").toInteger();
                QString content = obj.value("content").toString();
                qint64 fileID = obj.value("fileID").toInteger();
                qint64 msgID = obj.value("msgID").toInteger();                    qint64 receiver_id = obj.value("receiver_id").toInteger();
                qint64 senderId = obj.value("senderId").toInteger();
                qint64 sentAt = obj.value("sentAt").toInteger();
                DataBaseManage *dbm = DataBaseManage::instance();
                bool ok = dbm->addChatMessageAndUpdateRecent(QString::number(msgID),senderId,receiver_id,content,Type,sentAt,senderId,content,sentAt,1,0);
                qDebug() << "&AuthService::GetOfflineMessageSucceeded::for(const QJsonValue &val : list)";
            }
        }


        m_mw = new MainWindow();
        m_mw->setAttribute(Qt::WA_DeleteOnClose);
        auto agent = new QWK::WidgetWindowAgent(m_mw);
        agent->setup(m_mw);

        this->hide();
        m_mw->show();
        m_mw->setNetWork(m_sm);

    });

}

bool Widget::eventFilter(QObject *obj, QEvent *event)
{

    if (obj == ui->widget || obj == ui->widget_4) {
        QMouseEvent *e = static_cast<QMouseEvent*>(event);
        switch (event->type()) {
        case QEvent::MouseButtonPress:
            if (e->button() == Qt::LeftButton) {

                const QString platform = QGuiApplication::platformName();
                bool isWayland = platform.contains(QStringLiteral("wayland"), Qt::CaseInsensitive);
                QWindow *wh = window()->windowHandle();
                if (isWayland && wh) {
                    // startSystemMove() 会把拖动交给合成器（Wayland），不需要手工 move()
                    wh->startSystemMove();
                    return true;
                }

                m_dragging = true;
                m_dragPosition = e->globalPos() - this->frameGeometry().topLeft();
                return true;
            }
            break;

        case QEvent::MouseMove:
            if (m_dragging && (e->buttons() & Qt::LeftButton)) {
                move(e->globalPos() - m_dragPosition);
                return true;
            }
            break;

        case QEvent::MouseButtonRelease:
            m_dragging = false;
            return true;

        default:
            break;
        }
    }

    return QWidget::eventFilter(obj, event);
}


void Widget::on_config_cancel_Btn_clicked()
{
    ip->setIP(AppConfig::instance().getHost());
    ui->stackedWidget->setCurrentIndex(0);
}

