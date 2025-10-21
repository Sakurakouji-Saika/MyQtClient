#include "chatlistpage.h"
#include "ui_chatlistpage.h"
#include <QList>
#include "recent_chats/rc_line.h"


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
    QMap<QString,Recent_Data> m_list;

    // 压缩后的插入代码（更紧凑、易扩展）
    QDateTime now = QDateTime::currentDateTime();

    // 每项格式：{ avatar, message, id, name, seconds_offset_from_now, unread_count }
    struct Item { const char* avatar; const char* msg; const char* id; const char* name; int secs; int unread; };

    const Item items[] = {
        {":/picture/avatar/9.jpg", "文档已经更新完毕",            "1009", "Emma",  -259200, 55},
        {":/picture/avatar/1.jpg", "你好，最近怎么样？",            "1001", "张三",   0,      3},
        {":/picture/avatar/2.jpg", "会议资料已经发到你邮箱了",      "1002", "李四",  -300,    0},
        {":/picture/avatar/3.jpg", "[图片] 看看这个设计怎么样",     "1003", "王五",  -1800,   1},
        {":/picture/avatar/4.jpg", "周末一起吃饭吗？",             "1004", "赵六",  -7200,   0},
        {":/picture/avatar/5.jpg", "项目进度汇报已完成",           "1005", "Alice", -14400,  5},
        {":/picture/avatar/6.jpg", "代码审查通过了吗？",           "1006", "陈七",  -28800,  0},
        {":/picture/avatar/7.jpg", "谢谢你的帮助！",              "1007", "新用户", -86400,  2}, // 头像路径可为空时用默认头像处理
        {":/picture/avatar/8.jpg", "会议改到明天下午3点",          "1008", "冯八",  -172800, 0},
    };


    for (const auto &it : items) {
        QDateTime dt = now.addSecs(it.secs);
        m_list.insert(QString::fromUtf8(it.id),
                      Recent_Data(QString::fromUtf8(it.avatar),
                                  QString::fromUtf8(it.msg),
                                  QString::fromUtf8(it.id),
                                  QString::fromUtf8(it.name),
                                  dt,
                                  dt.toMSecsSinceEpoch(),
                                  it.unread));
    }

    populateRecentList(m_list);
}

void chatListPage::populateRecentList(const QMap<QString, Recent_Data> &recentList)
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
         qDebug() << "打开 item：" << index.data(user_id_Role).toString();
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


}

void chatListPage::onListItemClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    QString userId = index.data(user_id_Role).toString(); // 用你自定义的 role
    qDebug() << "item clicked (left):" << userId;



    int row = index.row();
    Recent_Data _temp = m_model->get_Row(row);
    _temp.UnreadCount = 0;
    m_model->update_Item_At(row,_temp);

    RC_Line *w = qobject_cast<RC_Line*>(ui->listView->indexWidget(index));
    if (w) {
        w->setUnReadOnZero();
    }


}


