#include "chatcontentpage.h"
#include "ui_chatcontentpage.h"

chatContentPage::chatContentPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatContentPage)
{
    ui->setupUi(this);
}

chatContentPage::~chatContentPage()
{
    delete ui;
}
