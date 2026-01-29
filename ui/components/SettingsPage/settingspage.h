#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>
#include "../../src/utils/styleLoader.h"

#include <QString>
namespace Ui {
class SettingsPage;
}

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = nullptr);
    ~SettingsPage();
    void setNikeName(QString name);

signals:
    void newNickNameSignals(QString &newNickName);
    void insertTestSignals();


private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::SettingsPage *ui;
};

#endif // SETTINGSPAGE_H
