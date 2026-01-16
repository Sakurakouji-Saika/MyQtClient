#ifndef FRIENDNOTIFY_PAGE_H
#define FRIENDNOTIFY_PAGE_H

#include <QWidget>
#include "fnp_line.h"
#include "FNPData.h"
#include <QListWidget>
#include "../Network/Service/servicemanager.h"
#include "../Network/Service/friendservice.h"
#include "../Network/Service/avatarservice.h"

namespace Ui { class FriendNotify_Page; }

class FriendNotify_Page : public QWidget
{
    Q_OBJECT

public:
    explicit FriendNotify_Page(QWidget *parent = nullptr);
    ~FriendNotify_Page();

    void setNetWork(ServiceManager *_sm);
    void GetData(qint64 uid);
    void test();
    void clearListWidget(QListWidget* listWidget);

    void removeLinesById(qint64 targetId);

private slots:
    void onGetFriendRequestListSuccess(QList<UserInfo> listData);

    void onSedAgreeFriend(qint64 _agreeUid);


signals:
    void updateFriendList();

private:
    Ui::FriendNotify_Page *ui;
    ServiceManager *m_sm = nullptr;
    FriendService *m_fs = nullptr;
    AvatarService *m_as = nullptr;
};

#endif // FRIENDNOTIFY_PAGE_H
