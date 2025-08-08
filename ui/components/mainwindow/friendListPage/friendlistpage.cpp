#include "friendlistpage.h"
#include "ui_friendlistpage.h"



friendListPage::friendListPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::friendListPage)
{
    ui->setupUi(this);

    // 1. 动态创建
    FriendListWidget *friendList = new FriendListWidget(this);

    // 2. 把它放到 placeHolderWidget 里
    //    如果 placeHolderWidget 已经有 layout：
    QLayout *lay = ui->placeHolderWidget->layout();
    if (!lay) {
        lay = new QVBoxLayout(ui->placeHolderWidget);
        lay->setContentsMargins(0,0,0,0);
    }
    lay->addWidget(friendList);

    // 3. 保证铺满
    friendList->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->placeHolderWidget->setContentsMargins(0,0,0,0);

    // 2. 添加分组数据：tuple<组名, 总人数, 在线人数>
    friendList->setGroups({
        { "我的好友",  3,  2 },
        { "同事",      2,  1 },
        { "家人",      1,  1 }
    });

    // 3. 为每个分组添加一些“模拟”好友
    //    真实项目中可以在 groupToggled 信号里异步加载
    QList<FriendInfo> friendsMy;  // “我的好友”
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });

    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });

    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u001", "Alice", "://picture/avatar/2.jpg", true });
    friendsMy.append({ "u002", "Bob",   "://picture/avatar/2.jpg",   false });
    friendsMy.append({ "u003", "Cici",  "://picture/avatar/2.jpg",  true });
    friendList->setFriendsForGroup("我的好友", friendsMy);

    QList<FriendInfo> friendsColleagues;  // “同事”
    friendsColleagues.append({ "u101", "David", "://picture/avatar/2.jpg", false });
    friendsColleagues.append({ "u102", "Emma",  "://picture/avatar/2.jpg",  true });
    friendList->setFriendsForGroup("同事", friendsColleagues);

    QList<FriendInfo> friendsFamily;  // “家人”
    friendsFamily.append({ "u201", "Mom",    "://picture/avatar/2.jpg",    true });
    friendList->setFriendsForGroup("家人", friendsFamily);

    // 4. 连接信号示例（可选）
    QObject::connect(friendList, &FriendListWidget::friendClicked,
                     [](const FriendInfo &fi){
                         qDebug() << "你点击了：" << fi.name << "(ID:" << fi.id << ")";
                     });

    QObject::connect(friendList, &FriendListWidget::groupToggled,
                     [&](const QString &grp, bool open){
                         qDebug() << (open ? "展开了分组：" : "收起了分组：") << grp;
                         // 如果你想在展开时再动态加载：
                         // if (open) { 调用后台接口，然后 friendList->setFriendsForGroup(grp, list); }
                     });


}

friendListPage::~friendListPage()
{
    delete ui;
}
