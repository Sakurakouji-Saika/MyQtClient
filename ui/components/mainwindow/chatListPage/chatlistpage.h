#ifndef CHATLISTPAGE_H
#define CHATLISTPAGE_H

#include <QWidget>

namespace Ui {
class chatListPage;
}

class chatListPage : public QWidget
{
    Q_OBJECT

public:
    explicit chatListPage(QWidget *parent = nullptr);
    ~chatListPage();

private:
    Ui::chatListPage *ui;
};

#endif // CHATLISTPAGE_H
