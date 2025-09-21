#ifndef FRIENDNOTIFY_PAGE_H
#define FRIENDNOTIFY_PAGE_H

#include <QWidget>

#include "fnp_line.h"
#include "FNPData.h"

namespace Ui {
class FriendNotify_Page;
}

class FriendNotify_Page : public QWidget
{
    Q_OBJECT

public:
    explicit FriendNotify_Page(QWidget *parent = nullptr);
    ~FriendNotify_Page();

    void test();

private:
    Ui::FriendNotify_Page *ui;
};

#endif // FRIENDNOTIFY_PAGE_H
