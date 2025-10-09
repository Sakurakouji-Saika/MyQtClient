#include "chatlistpage.h"
#include "ui_chatlistpage.h"
#include <qlist.h>
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
    ui->listWidget->setFocusPolicy(Qt::NoFocus);

    test();

    // 连接 itemClicked 信号
    connect(ui->listWidget, &QListWidget::itemClicked,
            this, &chatListPage::on_listWidget_itemClicked);




    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect(ui->listWidget, &QListWidget::customContextMenuRequested, [&](const QPoint &pos){
        showListContextMenu(pos);
    });
}

chatListPage::~chatListPage()
{
    delete ui;
}

void chatListPage::test()
{
    m_list.insert("1001", Recent_Data("://picture/avatar/1.jpg",  "你好",          "1001", "Alice",  "09:30", 3));
    m_list.insert("1002", Recent_Data("://picture/avatar/2.jpg",  "在吗？",        "1002", "Bob",    "10:05", 1));
    m_list.insert("1003", Recent_Data("://picture/avatar/3.jpg",  "明天开会",      "1003", "Cindy",  "11:20", 0));
    m_list.insert("1004", Recent_Data("://picture/avatar/4.jpg",  "收到文件了吗？","1004", "David",  "12:00", 2));
    m_list.insert("1005", Recent_Data("://picture/avatar/A.png",  "周末一起吃饭？","1005", "Eva",    "14:15", 5));
    m_list.insert("1006", Recent_Data("://picture/avatar/C3.png", "哈哈哈",        "1006", "Fiona",  "15:00", 0));
    m_list.insert("1007", Recent_Data("://picture/avatar/F9.png", "今晚打游戏",    "1007", "George", "16:40", 4));
    m_list.insert("1008", Recent_Data("://picture/avatar/1.jpg",  "下雨了",        "1008", "Helen",  "17:25", 1));
    m_list.insert("1009", Recent_Data("://picture/avatar/2.jpg",  "文件太大了",    "1009", "Ian",    "18:10", 6));
    m_list.insert("1010", Recent_Data("://picture/avatar/3.jpg",  "明天见",        "1010", "Jack",   "19:50", 0));

    populateRecentList(m_list);
}

void chatListPage::showListContextMenu(const QPoint &pos)
{
    qDebug() << "右键被触发";
    QListWidgetItem* item = ui->listWidget->itemAt(pos);
    if(!item) return;

    menu = new QMenu(this);
    // 关闭系统阴影并去掉框架（使用 Popup 类型）
    menu->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    // 允许透明背景（有助于边角过渡，虽然我们用 mask 裁形）
    menu->setAttribute(Qt::WA_TranslucentBackground);



    menu->setStyleSheet(R"(

    )");

    QAction* aOpen = menu->addAction("打开聊天");
    aOpen->setIcon(QIcon("://svg/102_open.svg"));

    QAction* aDel  = menu->addAction("删除");
    aDel->setIcon(QIcon("://svg/102_del.svg"));

    QAction* aCopy  = menu->addAction("复制账号");
    aCopy->setIcon(QIcon("://svg/102_copy.svg"));

    QAction* aCustom = menu->addAction("自定义操作");
    aCustom->setIcon(QIcon("://svg/102_other.svg"));

    QAction* act = menu->exec(ui->listWidget->mapToGlobal(pos));




    if(act == aOpen){
        QVariant v = item->data(Qt::UserRole);
        // 如果是 QString
        QString userIdStr = v.toString();

        QMessageBox::information(this, "Info", QString("打开: %1").arg(userIdStr));

    } else if(act == aDel){
        delete ui->listWidget->takeItem(ui->listWidget->row(item));
    }else if(act == aCopy){

        QVariant v = item->data(Qt::UserRole);
        // 如果是 QString
        QString userIdStr = v.toString();
        qDebug() << "复制ID为:" << userIdStr;


    }else if(act == aCustom){
        qDebug() << "自定义操作:" << item->text();
    }
}

void chatListPage::populateRecentList(const QMap<QString,Recent_Data> &recentList)
{
    ui->listWidget->clear();

    for(const Recent_Data &r : recentList){
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        RC_Line *lineWidget = new RC_Line(ui->listWidget);
        lineWidget->setData(r);

    // Don't fix the width here: let QListWidget manage the item's width so the
    // embedded widget can expand/shrink with the list (and splitter) changes.
    // We only set the height portion of the size hint.
    item->setSizeHint(QSize(0, 70));
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, lineWidget);
        item->setData(Qt::UserRole, r.user_id);
    }
}

void chatListPage::receiveMessage(const Recent_Data &msg)
{
    // 使用线程安全的方式调用
    QMetaObject::invokeMethod(this, "onNewMessage",
                              Qt::QueuedConnection,
                              Q_ARG(Recent_Data, msg));
}

void chatListPage::addRecent(const Recent_Data &r, bool toTop, bool select) {
    // 先更新内存数据
    m_list.insert(r.user_id, r);

    // 1) 新建 item 时不要传入 parent（不要 new QListWidgetItem(ui->listWidget)）
    QListWidgetItem *item = new QListWidgetItem();

    // 先创建 widget（不指定 parent，让 setItemWidget 负责 reparent）
    RC_Line *lineWidget = new RC_Line();
    lineWidget->setData(r);

    // 设定合适的 sizeHint（可以参考 widget 的 sizeHint）
    // Do not pin the width to the current viewport width. Use a zero width so
    // QListWidget can provide the available width; we only request the row height.
    item->setSizeHint(QSize(0, 70));
    // 或者： item->setSizeHint(lineWidget->sizeHint());

    // 插入到 list（此时 item 被 list 管理）
    if (toTop) {
        ui->listWidget->insertItem(0, item);
    } else {
        ui->listWidget->addItem(item);
    }

    // 把 widget 绑定到该 item（setItemWidget 会把 widget 的 parent 设为 list）
    ui->listWidget->setItemWidget(item, lineWidget);

    item->setData(Qt::UserRole, r.user_id);

    if (select) {
        int row = ui->listWidget->row(item);
        ui->listWidget->setCurrentRow(row);
        ui->listWidget->scrollToItem(item, QAbstractItemView::PositionAtCenter);
    }

    // 强制刷新（遇到奇怪显示问题可保留）
    ui->listWidget->update();
    ui->listWidget->viewport()->update();
}

void chatListPage::onNewMessage(const Recent_Data &msg)
{
    // 确保在主线程执行
    Q_ASSERT(QThread::currentThread() == this->thread());

    // 安全检查
    if (msg.user_id.isEmpty()) {
        qWarning() << "Received message with empty user_id";
        return;
    }

    if (m_list.contains(msg.user_id)) {
        // 更新内存数据
        Recent_Data curr(msg);
        m_list.insert(msg.user_id, curr);

        // 查找对应的 item
        QListWidgetItem *targetItem = nullptr;
        int targetRow = -1;

        for (int i = 0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem *item = ui->listWidget->item(i);
            if (item && item->data(Qt::UserRole).toString() == msg.user_id) {
                targetItem = item;
                targetRow = i;
                break;
            }
        }

        if (targetItem && targetRow >= 0) {
            // 获取当前绑定的 widget
            QWidget *w = ui->listWidget->itemWidget(targetItem);
            RC_Line *line = qobject_cast<RC_Line*>(w);

            // 更新 widget 显示
            if (line) {
                line->setData(curr);
            }

            // 保存当前选择状态
            QListWidgetItem *selectedItem = ui->listWidget->currentItem();
            QString selectedUserId;
            if (selectedItem) {
                selectedUserId = selectedItem->data(Qt::UserRole).toString();
            }

            // 移动到顶部
            if (targetRow > 0) { // 如果不在顶部才移动
                // 保存 widget 指针，因为 takeItem 会解除绑定
                QWidget *widgetToKeep = ui->listWidget->itemWidget(targetItem);

                // 取出 item（这会自动移除 widget 绑定）
                QListWidgetItem *takenItem = ui->listWidget->takeItem(targetRow);

                if (takenItem) {
                    // 插入到顶部
                    ui->listWidget->insertItem(0, takenItem);

                    // 重新设置 widget
                    if (widgetToKeep) {
                        ui->listWidget->setItemWidget(takenItem, widgetToKeep);
                    }

                    // 确保数据正确
                    takenItem->setData(Qt::UserRole, curr.user_id);
                }
            }

            // 恢复选择状态
            if (!selectedUserId.isEmpty()) {
                bool found = false;
                for (int i = 0; i < ui->listWidget->count(); ++i) {
                    QListWidgetItem *item = ui->listWidget->item(i);
                    if (item && item->data(Qt::UserRole).toString() == selectedUserId) {
                        ui->listWidget->setCurrentItem(item);
                        found = true;
                        break;
                    }
                }
                // 如果没找到之前选择的项，清除选择
                if (!found) {
                    ui->listWidget->clearSelection();
                }
            } else {
                ui->listWidget->clearSelection();
            }

            // // 高亮提示
            // if (line) {
            //     // 先保存当前样式，以便恢复
            //     QString originalStyle = line->styleSheet();
            //     line->setStyleSheet("background-color: rgba(255,240,180,0.9);");
            //     QTimer::singleShot(500, line, [line, originalStyle]() {
            //         if (line) {
            //             line->setStyleSheet(originalStyle);
            //         }
            //     });
            // }


        } else {
            qWarning() << "Found in m_list but not in listWidget:" << msg.user_id;
            // 如果数据存在但UI项不存在，重新创建
            addRecent(curr, true, false);
        }
    } else {
        // 新会话
        Recent_Data newr(msg);
        newr.UnreadCount = 1;
        addRecent(newr, true, false);

        // // 高亮新项
        // QListWidgetItem *newItem = ui->listWidget->item(0);

        // if (newItem) {
        //     QWidget *w = ui->listWidget->itemWidget(newItem);
        //     RC_Line *line = qobject_cast<RC_Line*>(w);
        //     if (line) {
        //         QString originalStyle = line->styleSheet();
        //         line->setStyleSheet("background-color: rgba(255,240,180,0.9);");
        //         QTimer::singleShot(500, line, [line, originalStyle]() {
        //             if (line) {
        //                 line->setStyleSheet(originalStyle);
        //             }
        //         });
        //     }
        // }


    }

    // 调试信息
    qDebug() << "Processed message from:" << msg.user_id
             << "Total items:" << ui->listWidget->count();
}


void chatListPage::on_listWidget_itemClicked(QListWidgetItem *item)
{
    if (!item) return;

    QString uid = item->data(Qt::UserRole).toString();
    qDebug() << "clicked uid =" << uid;

    QWidget *w = ui->listWidget->itemWidget(item);
    RC_Line *line = qobject_cast<RC_Line*>(w);

    if (line) {
        qDebug() << "got RC_Line widget for user:" << uid;

        auto it = m_list.find(uid);
        if(it != m_list.end()){
            // 创建副本修改，避免直接修改映射中的值
            Recent_Data updatedData = it.value();
            updatedData.UnreadCount = 0;

            // 更新内存数据
            m_list.insert(uid, updatedData);

            // 更新显示
            line->setData(updatedData);
        }
    }
}
