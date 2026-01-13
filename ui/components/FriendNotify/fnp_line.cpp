#include "fnp_line.h"
#include "ui_fnp_line.h"

#include "../../src/utils/utils.h"
FNP_Line::FNP_Line(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FNP_Line)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_StyledBackground, true);
    StyleLoader::loadWidgetStyle(this, ":/styles/FriendNotify_Page.css");

}

FNP_Line::~FNP_Line()
{
    delete ui;
}


void FNP_Line::setData(const QString &str_uid, const QString &userName, const QString &timeText, const qint64 &_uid)
{

    ui->qqNumber->setText(str_uid);
    ui->userName->setText(userName);
    ui->timeText->setText(timeText);

    ui->avatarIcon->setAvatar(_uid,45);
}
