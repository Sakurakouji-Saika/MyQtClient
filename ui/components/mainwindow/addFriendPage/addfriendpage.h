#ifndef ADDFRIENDPAGE_H
#define ADDFRIENDPAGE_H

#include <QWidget>

namespace Ui {
class addFriendPage;
}

class addFriendPage : public QWidget
{
    Q_OBJECT

public:
    explicit addFriendPage(QWidget *parent = nullptr);
    ~addFriendPage();

private:
    Ui::addFriendPage *ui;
};

#endif // ADDFRIENDPAGE_H
