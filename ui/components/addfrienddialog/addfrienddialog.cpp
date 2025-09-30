#include "addfrienddialog.h"
#include "ui_addfrienddialog.h"

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
    // 如果想隐藏整个 AF_userInfo：
    if (!ui->AF_userInfo) return;
    ui->AF_userInfo->setVisible(true);


}
