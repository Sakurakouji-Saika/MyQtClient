#include "friendnotify_page.h"
#include "ui_friendnotify_page.h"
#include "../../src/utils/styleLoader.h"
#include "../../utils/appconfig.h"

FriendNotify_Page::FriendNotify_Page(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FriendNotify_Page)
{
    ui->setupUi(this);
    StyleLoader::loadWidgetStyle(this, ":/styles/FriendNotify_Page.css");



    test();


}

FriendNotify_Page::~FriendNotify_Page()
{
    delete ui;
}

void FriendNotify_Page::setNetWork(ServiceManager *_sm)
{
    m_sm = _sm;
    m_fs= m_sm->friendApi();
}

void FriendNotify_Page::GetData(qint64 uid)
{
    m_fs->get_Friend_request(uid);

    connect(m_fs,&FriendService::GetFriendRequestListSuccessSignals,this,[](){

    });
}

void FriendNotify_Page::test()
{

    GetData(AppConfig::instance().getUserID());

    // 1. 准备数据（可从文件、数据库或网络读取）
    std::vector<FNPData> dataList;
    dataList.emplace_back("://picture/avatar/1.jpg", "10001", "张三", "2025-09-19 15:00");
    dataList.emplace_back("://picture/avatar/1.jpg", "10002", "李四AA", "2025-09-19 15:10");
    dataList.emplace_back("D://Documents//Tencent Files//211949940//nt_qq//nt_data//Pic//2025-09//Ori//e4d8aa49f1cd6751c9575f58079acdf8.jpg", "10003", "王五", "2025-09-19 15:20");

    ui->FNP_listWidget->setSpacing(10);
    // 1. 禁止选择
    ui->FNP_listWidget->setSelectionMode(QAbstractItemView::NoSelection);



    for (const auto &d : dataList) {
        // 创建自定义 widget
        FNP_Line *line = new FNP_Line;
        line->setData(d.avatarPath,d.qq,d.userName,d.timeText);
        line->setFixedHeight(70); // 固定高度，便于 item->setSizeHint

        // 创建列表项并把 widget 放进去
        QListWidgetItem *item = new QListWidgetItem(ui->FNP_listWidget);
        // 设置 item 大小（宽度由 QListWidget 管理）
        item->setSizeHint(line->sizeHint());

        ui->FNP_listWidget->addItem(item);
        ui->FNP_listWidget->setItemWidget(item, line);
    }

}
