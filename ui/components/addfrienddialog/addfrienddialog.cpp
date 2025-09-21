#include "addfrienddialog.h"
#include "ui_addfrienddialog.h"

addfrienddialog::addfrienddialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::addfrienddialog)
{
    ui->setupUi(this);
        StyleLoader::loadWidgetStyle(this, ":/styles/addfrienddialog.css");

}

addfrienddialog::~addfrienddialog()
{
    delete ui;
}
