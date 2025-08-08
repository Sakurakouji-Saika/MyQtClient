#ifndef USERINFOPAGE_H
#define USERINFOPAGE_H

#include <QWidget>

namespace Ui {
class userInfoPage;
}

class userInfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit userInfoPage(QWidget *parent = nullptr);
    ~userInfoPage();

private:
    Ui::userInfoPage *ui;
};

#endif // USERINFOPAGE_H
