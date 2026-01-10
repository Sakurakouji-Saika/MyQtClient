#ifndef FRIENDNOTIFY_PAGE_H
#define FRIENDNOTIFY_PAGE_H

#include <QWidget>

#include "fnp_line.h"
#include "FNPData.h"
#include "../Network/Service/servicemanager.h"
#include "../Network/Service/friendservice.h"

namespace Ui {
class FriendNotify_Page;
}

class FriendNotify_Page : public QWidget
{
    Q_OBJECT

public:
    explicit FriendNotify_Page(QWidget *parent = nullptr);
    ~FriendNotify_Page();

    void setNetWork(ServiceManager *_sm);

    void GetData(qint64 uid);

    void test();

private:
    Ui::FriendNotify_Page *ui;

    ServiceManager *m_sm;
    FriendService *m_fs;
};

#endif // FRIENDNOTIFY_PAGE_H
