#include "friendnotify_page.h"
#include "ui_friendnotify_page.h"
#include "../../src/utils/styleLoader.h"
#include "../../utils/appconfig.h"

#include <QList>
#include "../../src/Network/models/userinfo.h"
#include "../../src/utils/utils.h"
#include "../../DataBaseManage/databasemanage.h"

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

    connect(m_fs,
            &FriendService::GetFriendRequestListSuccessSignals,
            this,
            &FriendNotify_Page::onGetFriendRequestListSuccess);

    connect(m_fs,&FriendService::InsetFriendData,this,[this](qint64 uid,QString avatar,QString nickname,QString username,qint64 file_avatar_id){

        // 下载头像
        m_as->requestAvatarByFileId(QString::number(file_avatar_id));

        // 插入到数据库
        DataBaseManage *dbm = DataBaseManage::instance();
        dbm->upsertFriend(uid,username,nickname,QString(),file_avatar_id,avatar,0);

        emit updateFriendList();

        removeLinesById(uid);
    });
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

void FriendNotify_Page::removeLinesById(qint64 targetId)
{
    int count = ui->FNP_listWidget->count();
    for (int i = count - 1; i >= 0; --i) {              // 倒序遍历，防止索引位移
        QListWidgetItem *item = ui->FNP_listWidget->item(i);
        if (!item) continue;

        QWidget *w = ui->FNP_listWidget->itemWidget(item);
        FNP_Line *line = qobject_cast<FNP_Line*>(w);

        if (line && line->m_uid == targetId) {
            ui->FNP_listWidget->removeItemWidget(item);
            delete line;
            delete ui->FNP_listWidget->takeItem(i);
        }
    }
}

void FriendNotify_Page::onGetFriendRequestListSuccess(QList<UserInfo> listData)
{
    clearListWidget(ui->FNP_listWidget);

    for (const auto &item : listData) {
        FNP_Line *line = new FNP_Line;


        // 将子窗口的发送同意信号发送到这里的槽函数，发送网络请求（懒得再把网络指针传递到FNP_Line 内了。
        connect(line,&FNP_Line::agreeFriendClicked,this,&FriendNotify_Page::onSedAgreeFriend);

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

void FriendNotify_Page::onSedAgreeFriend(qint64 _agreeUid)
{
    m_fs->send_agree_friend(_agreeUid);
}
