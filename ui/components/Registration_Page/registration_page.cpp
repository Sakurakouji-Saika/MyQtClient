#include "registration_page.h"
#include "ui_registration_page.h"

Registration_Page::Registration_Page(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Registration_Page)
{
    ui->setupUi(this);

    StyleLoader::loadWidgetStyle(this, ":/styles/registration_page.css");
}

Registration_Page::~Registration_Page()
{
    delete ui;
}
