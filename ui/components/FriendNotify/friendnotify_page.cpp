#include "friendnotify_page.h"
#include "ui_friendnotify_page.h"
#include "../../src/utils/styleLoader.h"
#include "../../utils/appconfig.h"

#include <QList>
#include "../../src/Network/models/userinfo.h"
#include "../../src/utils/utils.h"

FriendNotify_Page::FriendNotify_Page(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FriendNotify_Page)
{
    ui->setupUi(this);
    StyleLoader::loadWidgetStyle(this, ":/styles/FriendNotify_Page.css");

    // 可选：初始设置列表属性（一次）
    ui->FNP_listWidget->setSpacing(10);
    ui->FNP_listWidget->setSelectionMode(QAbstractItemView::NoSelection);
}

FriendNotify_Page::~FriendNotify_Page()
{
    delete ui;
}

void FriendNotify_Page::setNetWork(ServiceManager *_sm)
{
    m_sm = _sm;
    m_fs = m_sm->friendApi();
    m_as = m_sm->avatar();

    // 只连接一次：使用 UniqueConnection 防止重复连接
    connect(m_fs,
            &FriendService::GetFriendRequestListSuccessSignals,
            this,
            &FriendNotify_Page::onGetFriendRequestListSuccess);
}

void FriendNotify_Page::GetData(qint64 uid)
{

    if (!m_fs) return;
    m_fs->get_Friend_request(uid);
}

void FriendNotify_Page::test()
{
    GetData(AppConfig::instance().getUserID());
}

void FriendNotify_Page::clearListWidget(QListWidget *listWidget)
{
    while (listWidget->count() > 0) {
        QListWidgetItem* item = listWidget->takeItem(0);
        if (!item) continue;

        QWidget* widget = listWidget->itemWidget(item);
        if (widget) {
            widget->deleteLater();
        }
        delete item;
    }
}

void FriendNotify_Page::onGetFriendRequestListSuccess(QList<UserInfo> listData)
{
    clearListWidget(ui->FNP_listWidget);

    for (const auto &item : listData) {
        FNP_Line *line = new FNP_Line;

        line->setData(
                      QString::number(item.userId),
                      QString(item.username),
                      formatMsToYMDHM(item.created_at),
                      item.userId
                      );

        line->setFixedHeight(70);


        QListWidgetItem *wItem = new QListWidgetItem(ui->FNP_listWidget);
        wItem->setSizeHint(line->sizeHint());
        ui->FNP_listWidget->addItem(wItem);

        ui->FNP_listWidget->setItemWidget(wItem, line);


        // yi bu xia zhai tou xiang
        m_as->RequestAvatarInfoByUserID(item.userId);
    }
}
