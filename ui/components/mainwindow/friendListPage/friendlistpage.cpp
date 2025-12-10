#include "friendlistpage.h"
#include "ui_friendlistpage.h"

#include "friendlistpage.h"
#include "ui_friendlistpage.h"
#include "../../friendListPage/friendlistwidget.h"  // 请确保头文件路径正确

#include "../../Src/DataBaseManage/databasemanage.h"
#include "../../Src/DataBaseManage/model/FriendInfo.h"     // FriendInfo 定义的头文件

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

    // 4. 连接信号示例（可选）
    QObject::connect(friendList, &FriendListWidget::friendClicked,
                     [this](const FriendInfo &fi){
                         qDebug() << "你点击了：" << fi.username << "(ID:" << fi.friendId << ")";

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
