#include "settingspage.h"
#include "ui_settingspage.h"
#include "../../src/utils/appconfig.h"

SettingsPage::SettingsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsPage)
{
    ui->setupUi(this);


    StyleLoader::loadWidgetStyle(this, ":/styles/SettingsPage.css");

    ui->label_4->setText(QString::number(AppConfig::instance().getUserID()));


}

SettingsPage::~SettingsPage()
{
    delete ui;
}

void SettingsPage::setNikeName(QString name)
{
    ui->label_5->setText(name);
}






void SettingsPage::on_pushButton_clicked()
{
    QString newName =ui->lineEdit->text();
    emit newNickNameSignals(newName);
}


void SettingsPage::on_pushButton_2_clicked()
{
    emit insertTestSignals();
}

