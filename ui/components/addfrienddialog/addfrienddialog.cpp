#include "addfrienddialog.h"
#include "ui_addfrienddialog.h"
#include <QMessageBox>


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

    if(DataBaseManage::instance()->isFriend(user_id.toInt())){
        QMessageBox msgBox;
        msgBox.setText("该账号已是好友");
        msgBox.exec();
        return;
    }

    // 如果想隐藏整个 AF_userInfo：
    if (!ui->AF_userInfo) return;
    ui->AF_userInfo->setVisible(true);

    



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



    ui->Af_user_2_lab->setText(name);
    ui->Af_user_2_lab_2->setText("ID:" + QString::number(id));



}

void addfrienddialog::on_AF_use_2_btn_clicked()
{

}

