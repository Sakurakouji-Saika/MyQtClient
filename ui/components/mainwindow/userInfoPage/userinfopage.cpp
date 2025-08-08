#include "userinfopage.h"
#include "ui_userinfopage.h"

userInfoPage::userInfoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::userInfoPage)
{
    ui->setupUi(this);
}

userInfoPage::~userInfoPage()
{
    delete ui;
}
