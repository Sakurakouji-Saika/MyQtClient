#include "profilepage_main.h"
#include "ui_profilepage_main.h"

#include <QDir>

#include "../../src/utils/styleLoader.h"
#include "../../src/utils/appconfig.h"
#include "../../src/DataBaseManage/databasemanage.h"

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

void ProfilePage_Main::setNetWork(ServiceManager *_sm)
{
    m_sm = _sm;
    m_fs = m_sm->friendApi();
    connect(m_fs,&FriendService::DeleteFriendSuccessSignals,this,&ProfilePage_Main::on_DeleteFriendSuccessSignals);
}

void ProfilePage_Main::on_btn_push_msg_clicked(){
    emit open_friend_chat_page(m_info);
    qDebug("on_btn_push_msg_clicked 打开聊天窗口被点击");
}


void ProfilePage_Main::on_btn_clear_friend_clicked()
{
    m_fs->Delete_friend(m_info.friendId);
}

void ProfilePage_Main::on_DeleteFriendSuccessSignals(qint64 delete_uid)
{
    // 先删除本地数据库
    DataBaseManage* dbm = DataBaseManage::instance();
    bool ok = dbm->deleteFriendByUID(delete_uid);


    qDebug() << "ProfilePage_Main::on_DeleteFriendSuccessSignals::已触发" << ok  << "\t uid:" << delete_uid;

    // 在发送信号，让mainwindow的函数里面重新加载数据库。
    if(ok){
        emit deleteFriendUid(delete_uid);
    }
}

