#include "settingspage.h"
#include "ui_settingspage.h"
#include "../../src/utils/appconfig.h"
#include <QIntValidator>

SettingsPage::SettingsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsPage)
{
    ui->setupUi(this);


    ui->lineEdit_2->setValidator(
        new QIntValidator(0, 100000, ui->lineEdit_2)
        );

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
    int num = ui->lineEdit_2->text().toInt(0);

    AppConfig::instance().setNumberBubbles(num);
    emit insertTestSignals();
}

