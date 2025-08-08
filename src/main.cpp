#include "app/widget.h"

#include "utils/StyleLoader.h"
#include <QApplication>
#include "utils/appconfig.h"
#include "app/mainwindow.h"
#include "../ui/components/contacts/FriendInfo.h"
#include "../ui/components/contacts/friendlistwidget.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    StyleLoader::setDebugEnabled(false);
    StyleLoader::setDebugResourceRoot("D:/Documents/Qt/MyClient/src/resources");

    // 一次性加载配置，之后只读
    AppConfig::initialize("://config/app.ini");


    // Widget w;
    // w.show();

    MainWindow m;
    m.show();





    // 1. 创建 FriendListWidget
    FriendListWidget *friendList = new FriendListWidget;
    friendList->setWindowTitle("好友列表演示");

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

    // // 5. 显示并进入事件循环
    // friendList->resize(300, 500);
    // friendList->show();



    return a.exec();
}
