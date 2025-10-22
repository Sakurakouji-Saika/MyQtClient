#include "friendlistpage.h"
#include "ui_friendlistpage.h"

#include "friendlistpage.h"
#include "ui_friendlistpage.h"
#include "../../friendListPage/friendlistwidget.h"  // 请确保头文件路径正确
#include "../../Src/DataBaseManage/model/FriendInfo.h"     // FriendInfo 定义的头文件

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


    // 2. 添加分组数据：tuple<组名, 总人数, 在线人数>
    friendList->setGroups({
        { "我的好友",   3,  2 },
        { "同事",      2,  1 },
        { "家人",      1,  1 }
    });

    // 3. 为每个分组添加一些“模拟”好友
    //    真实项目中可以在 groupToggled 信号里异步加载
    QList<FriendInfo> friendsMy;  // “我的好友”
    // ✅ 显式构造（OK）
    friendsMy.append(FriendInfo( -1, "uid_001", "Alice", "://picture/avatar/9.jpg", 1, "同事", QDateTime::currentSecsSinceEpoch()));

    // ✅ 批量随机生成
    for (int i = 1; i <= 5; ++i) {
        friendsMy.append(FriendInfo(
            i + 1,
            QString("uid_%1").arg(i + 1),
            QString("User%1").arg(i + 1),
            QString("://picture/avatar/%1.jpg").arg(i + 1),
            i % 2,
            "备注",
            QDateTime::currentSecsSinceEpoch() - i * 100
            ));
        qDebug() <<"://picture/avatar/" + QString(i) + ".jpg";
    }

    friendList->setFriendsForGroup("我的好友", friendsMy);

    QList<FriendInfo> friendsColleagues;  // “同事”


    QList<FriendInfo> friendsFamily;  // “家人”


    friendList->setFriendsForGroup("家人", friendsFamily);

    // 4. 连接信号示例（可选）
    QObject::connect(friendList, &FriendListWidget::friendClicked,
                     [this](const FriendInfo &fi){
                        qDebug() << "你点击了：" << fi.display_name << "(ID:" << fi.friend_id << ")";

                        emit signals_open_profile_page(fi);

                        // if(m_profile_main != nullptr){
                        //     delete m_profile_main;
                        // }

                        // m_profile_main = new ProfilePage_Main(this);
                        // m_profile_main->addInfo(fi);
                        // m_profile_main->show();

                        // connect(m_profile_main,&ProfilePage_Main::open_friend_chat_page,[]{
                        //     qDebug() << "12312312312";
                        // });

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
