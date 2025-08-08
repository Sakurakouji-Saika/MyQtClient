#include "placeholderpage.h"
#include "ui_placeholderpage.h"

placeholderPage::placeholderPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::placeholderPage)
{
    ui->setupUi(this);
}

placeholderPage::~placeholderPage()
{
    delete ui;
}
