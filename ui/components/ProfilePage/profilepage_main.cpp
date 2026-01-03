#include "profilepage_main.h"
#include "ui_profilepage_main.h"

#include <QDir>

#include "../../src/utils/styleLoader.h"
#include "../../src/utils/appconfig.h"

ProfilePage_Main::ProfilePage_Main(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProfilePage_Main)
{
    ui->setupUi(this);
    StyleLoader::loadWidgetStyle(this, ":/styles/profilepage_main.css");

}

ProfilePage_Main::~ProfilePage_Main()
{
    delete ui;
}

void ProfilePage_Main::addInfo(FriendInfo info)
{

    ui->State->setText(info.status?"在线":"离线");
    ui->name->setText(info.nickname);
    ui->id->setText(info.username);

    QSize avatarSize;
    avatarSize.setWidth(90);
    avatarSize.setHeight(90);

    ui->Avatar->setAvatar(info.friendId,90);
    m_info = info;

}

void ProfilePage_Main::on_btn_push_msg_clicked(){
    emit open_friend_chat_page(m_info);
    qDebug("on_btn_push_msg_clicked 打开聊天窗口被点击");
}

