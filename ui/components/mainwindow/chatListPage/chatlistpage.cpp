#include "chatlistpage.h"
#include "ui_chatlistpage.h"
#include <QList>
#include "recent_chats/rc_line.h"

#include "../../src/DataBaseManage/databasemanage.h"
#include "../../src/DataBaseManage/model/FriendInfo.h"

#include <QTimer>
#include <QThread>
#include <QMessageBox>

chatListPage::chatListPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatListPage)
{
    ui->setupUi(this);
    StyleLoader::loadWidgetStyle(this, ":/styles/recent_chats.css");

    ui->listView->setFocusPolicy(Qt::NoFocus);


    m_model = new Model(this);
    ui->listView->setModel(m_model);
    ui->listView->setUniformItemSizes(true);
    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);

    m_model->my_diy_sort(true); // 自定义排序顺序

    test();

    // 当 model 发出 dataChanged 时，更新对应的 indexWidget（如果存在）
    connect(m_model, &QAbstractItemModel::dataChanged, this, &chatListPage::onModelDataChanged);


    // 启用自定义右键菜单
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView,&QListView::customContextMenuRequested,[this](const QPoint &pos){
        on_showListContextMenu(pos);

    });


    connect(ui->listView, &QListView::clicked,this, &chatListPage::onListItemClicked);

    // 自动隐藏导航条
    new AutoHideScrollbar(ui->listView, 10000, ui->listView);

}

chatListPage::~chatListPage()
{
    delete ui;
}

void chatListPage::test()
{

    QList<Recent_Data> m_list_2;

    DataBaseManage *mgr = DataBaseManage::instance();

    QList<RecentMessage>temp =  mgr->getRecentMessageList();


    for(int i=0;i<temp.size();i++){

        qDebug()  << "temp[i].peer_id" << temp[i].peer_id;
        Recent_Data t;


        t.UnreadCount = (temp[i].unread_count>99 ? 99 : temp[i].unread_count);

        std::optional<FriendInfo> _temp_avatar_data = mgr->GetFriendAvatarById(temp[i].peer_id);

        if(_temp_avatar_data.has_value()){
            if(!_temp_avatar_data.value().avatar.isEmpty()){
                t.avatarPath = _temp_avatar_data.value().avatar;
            }else{
                t.avatarPath = QString();
            }
        }else{
            t.avatarPath = QString();
        }



        qDebug() << "chatListPage::test()::t.avatarPath::" << t.avatarPath;
        qDebug() << "chatListPage::test()::t.avatarPath::用户ID：" << temp[i].peer_id;

        t.msg = temp[i].last_msg;
        t.msg_time = QDateTime::fromSecsSinceEpoch(temp[i].last_time);
        t.user_id = temp[i].peer_id;
        t.userName = mgr->getDisplayNameByFriendId(temp[i].peer_id);
        t.timestamp = temp[i].last_time;
        m_list_2.append(t);
    };

    populateRecentList(m_list_2);

}

void chatListPage::populateRecentList(const QList<Recent_Data> &recentList)
{
    for(const Recent_Data &r:recentList){
        m_model->addItem(r);
    }

    for(int row = 0; row < m_model->rowCount();++row){
        QModelIndex idx = m_model->index(row,0);
        RC_Line *w = new RC_Line;
        w->setFixedHeight(72);              // 强制 widget 高度
        w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        w->setData(m_model->data(idx,all_data_Role));
        ui->listView->setIndexWidget(idx,w);
    }


}

void chatListPage::on_showListContextMenu(const QPoint &pos)
{

    QModelIndex index = ui->listView->indexAt(pos);  // 判断鼠标点击的项

    if (!index.isValid()) {
        // 点击在空白处，不弹出或弹出空白菜单
        return;
    }

    menu = new QMenu(this);
    // 关闭系统阴影并去掉框架（使用 Popup 类型）
    menu->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    // 允许透明背景（有助于边角过渡，虽然我们用 mask 裁形）
    menu->setAttribute(Qt::WA_TranslucentBackground);


    QAction* aOpen = menu->addAction("打开聊天");
    aOpen->setIcon(QIcon("://svg/102_open.svg"));

    QAction* aDel  = menu->addAction("删除");
    aDel->setIcon(QIcon("://svg/102_del.svg"));

    QAction* aCopy  = menu->addAction("复制账号");
    aCopy->setIcon(QIcon("://svg/102_copy.svg"));

    QAction* aCustom = menu->addAction("自定义操作");
    aCustom->setIcon(QIcon("://svg/102_other.svg"));

    QAction* act = menu->exec(ui->listView->mapToGlobal(pos));




    if(act == aOpen){
        qDebug() << "打开 item：" << index.data(user_id_Role).toInt();
        emit openChatPage(index.data(user_id_Role).toInt());

    } else if(act == aDel){
        // 删除选中项：移除 listView 中的 index widget（如果有），并从 model 与内部 map 中删除
        int row = index.row();
        if (row >= 0) {
            // 1) 如果使用了 setIndexWidget，先取出并删除 widget
            QWidget *w = ui->listView->indexWidget(index);
            if (w) {
                ui->listView->setIndexWidget(index, nullptr);
                w->deleteLater();

            }

            // 3) 从 model 删除行
            if (m_model) {
                m_model->removeRow(row);
            }
        }
    }else if(act == aCopy){
        QClipboard *clip = QApplication::clipboard();
        clip->setText(index.data(user_id_Role).toString());

    }else if(act == aCustom){

    }
}

void chatListPage::receiveMessage(const Recent_Data &msg)
{
    // 使用线程安全的方式调用
    QMetaObject::invokeMethod(this, "onNewMessage",
                              Qt::QueuedConnection,
                              Q_ARG(Recent_Data, msg));
}

void chatListPage::receiveMessages(const QList<Recent_Data> &list)
{
    if (list.isEmpty()) return;
    m_model->addItems(list);
}


void chatListPage::deleteItemWidgetByUid(qint64 uid)
{
    for (int row = 0; row < m_model->rowCount(); ++row) {
        QModelIndex idx = m_model->index(row, 0);
        qint64 itemUid = idx.data(user_id_Role).toLongLong();

        if (itemUid == uid) {
            QWidget *w = ui->listView->indexWidget(idx);
            if (w) {
                ui->listView->setIndexWidget(idx, nullptr);
                w->deleteLater();
            }
            m_model->removeRow(row);
            qDebug() << "成功删除uid为" << uid << "的聊天项";
            return;
        }
    }
    qDebug() << "未找到uid为" << uid << "的聊天项";
}



void chatListPage::onNewMessage(const Recent_Data &msg)
{

    m_model->addItemFront(msg);

    // 创建对应的 RC_Line 并放在 index 0
    QModelIndex idx = m_model->index(0, 0);
    RC_Line *w = new RC_Line;
    w->setFixedHeight(72);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    w->setData(m_model->data(idx, all_data_Role));

    ui->listView->setIndexWidget(idx, w);
    ui->listView->update();

}

void chatListPage::onListItemClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    int userId = index.data(user_id_Role).toInt();
    qDebug() << "item clicked (left):" << userId;



    int row = index.row();
    Recent_Data _temp = m_model->get_Row(row);
    _temp.UnreadCount = 0;
    // m_model->update_Item_At(row,_temp);

    RC_Line *w = qobject_cast<RC_Line*>(ui->listView->indexWidget(index));
    if (w) {
        w->setUnReadOnZero();
    }

    emit openChatPage(index.data(user_id_Role).toInt());
}

void chatListPage::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (!topLeft.isValid()) return;

    int start = topLeft.row();
    int end = bottomRight.row();

    for (int r = start; r <= end; ++r) {

        QModelIndex idx = m_model->index(r, 0);
        if (!idx.isValid()) continue;

        QVariant v = m_model->data(idx, all_data_Role);
        RC_Line *w = qobject_cast<RC_Line*>(ui->listView->indexWidget(idx));

        if (w) {

            w->setData(v);
        } else {

            // 如果没有 widget，则创建并绑定，确保 UI 始终与 model 一致
            RC_Line *nw = new RC_Line;
            nw->setFixedHeight(72);
            nw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            nw->setData(v);
            ui->listView->setIndexWidget(idx, nw);
        }
    }
    ui->listView->update();
}


