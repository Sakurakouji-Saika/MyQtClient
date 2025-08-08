#include "chatlistpage.h"
#include "ui_chatlistpage.h"

chatListPage::chatListPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatListPage)
{
    ui->setupUi(this);
}

chatListPage::~chatListPage()
{
    delete ui;
}
