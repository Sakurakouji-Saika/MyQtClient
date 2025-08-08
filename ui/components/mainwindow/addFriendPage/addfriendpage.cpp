#include "addfriendpage.h"
#include "ui_addfriendpage.h"

addFriendPage::addFriendPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::addFriendPage)
{
    ui->setupUi(this);
}

addFriendPage::~addFriendPage()
{
    delete ui;
}
