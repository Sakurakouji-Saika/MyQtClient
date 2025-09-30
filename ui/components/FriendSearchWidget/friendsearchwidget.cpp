#include "friendsearchwidget.h"
#include "ui_friendsearchwidget.h"

FriendSearchWidget::FriendSearchWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FriendSearchWidget)
{
    ui->setupUi(this);
}

FriendSearchWidget::~FriendSearchWidget()
{
    delete ui;
}
