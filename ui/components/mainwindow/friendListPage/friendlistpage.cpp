#include "friendlistpage.h"
#include "ui_friendlistpage.h"

#include "friendlistpage.h"
#include "ui_friendlistpage.h"
#include "../../friendListPage/friendlistwidget.h"  // 请确保头文件路径正确

#include "../../src/DataBaseManage/databasemanage.h"
#include "../../src/DataBaseManage/model/FriendInfo.h"     // FriendInfo 定义的头文件

friendListPage::friendListPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::friendListPage)
{
    ui->setupUi(this);

    init();
}
friendListPage::~friendListPage()
{
    delete ui;
}

void friendListPage::init()
{
    FriendListWidget *friendList = new FriendListWidget(this);

    QLayout *lay = ui->placeHolderWidget->layout();
    if (!lay) {
        lay = new QVBoxLayout(ui->placeHolderWidget);
        lay->setContentsMargins(0,0,0,0);
    }
    lay->addWidget(friendList);


    friendList->setGroups({
        { "我的好友",   DataBaseManage::instance()->getFriendList().size(), 0  },
        { "同事",      0,  0 },
        { "家人",      0,  0 }
    });


    QList<FriendInfo> friendsMy  = DataBaseManage::instance()->getFriendList();


    friendList->setFriendsForGroup("我的好友", friendsMy);

    QList<FriendInfo> friendsColleagues;  // “同事”


    QList<FriendInfo> friendsFamily;  // “家人”


    friendList->setFriendsForGroup("家人", friendsFamily);

    QObject::connect(friendList, &FriendListWidget::friendClicked,
                     [this](const FriendInfo &fi){
                         qDebug() << "你点击了：" << fi.username << "(ID:" << fi.friendId << ")";

                         emit signals_open_profile_page(fi);
                     });

    QObject::connect(friendList, &FriendListWidget::groupToggled,
                     [&](const QString &grp, bool open){
                         qDebug() << (open ? "展开了分组：" : "收起了分组：") << grp;
                     });
}
