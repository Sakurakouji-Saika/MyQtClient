#include "addfrienddialog.h"
#include "ui_addfrienddialog.h"
#include <QMessageBox>
#include "../../src/Network/Service/servicemanager.h"
#include "../../Network/Service/friendservice.h"

addfrienddialog::addfrienddialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::addfrienddialog)
{
    ui->setupUi(this);
    StyleLoader::loadWidgetStyle(this, ":/styles/addfrienddialog.css");
    ui->AF_userInfo->setVisible(false);

    QSize avatarSize;
    avatarSize.setWidth(40);
    avatarSize.setHeight(40);
    ui->af_user_2_avatar->setPixmap(scaledRoundedPixmap(QPixmap("://picture/avatar/A.png"),avatarSize,40));

}

addfrienddialog::~addfrienddialog()
{
    delete ui;
}

void addfrienddialog::setNetWork(ServiceManager *_sm)
{
    m_sm =_sm;
}



void addfrienddialog::on_add_friend_btn_clicked()
{
    // 开始查找搜索账号
    QString user_id = ui->AF_lineEdit->text();

    if(user_id.isEmpty()){
        QMessageBox msgBox;
        msgBox.setText("搜索账号不能为空");
        msgBox.exec();
        return;
    }

    // if(DataBaseManage::instance()->isFriend(user_id.toInt())){
    //     QMessageBox msgBox;
    //     msgBox.setText("该账号已是好友");
    //     msgBox.exec();
    //     return;
    // }

    // 如果想隐藏整个 AF_userInfo：
    if (!ui->AF_userInfo) return;
    ui->AF_userInfo->setVisible(true);

    FriendService *m_fs = m_sm->friendApi();
    m_fs->search_friends(user_id.toLongLong());

    connect(m_fs,&FriendService::SearchFriednSuccessSignals,this,[this](qint64 uid, QString userName, QString nickname, qint64 avatar_file_id, QString avatar){
        qDebug() << "addfrienddialog::on_add_friend_btn_clicked::username::" << userName;
        ui->Af_userName->setText(userName);

    });

}






// bu zhi dao you shen men yong de han shu
void addfrienddialog::on_return_addFriend_Info(const QJsonValue &info)
{
    qDebug() << "void addfrienddialog::on_return_addFriend_Info(const QJsonValue &info) 触发";
    QJsonObject dataObj = info.toObject();
    int id = dataObj.value("id").toInt();
    QString name = dataObj.value("name").toString();
    QString avatar = dataObj.value("head").toString();
    int status = dataObj.value("status").toInt();


    QString headAvatar = dataObj.value("head").toString();

    QJsonObject json;
    json.insert("from", name);
    json.insert("id", -2);



    ui->Af_userName->setText(name);
    ui->Af_userID->setText("ID:" + QString::number(id));



}

void addfrienddialog::on_AF_use_2_btn_clicked()
{

}

