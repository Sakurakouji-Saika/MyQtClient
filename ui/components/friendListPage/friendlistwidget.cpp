// FriendListWidget.cpp
#include "friendlistwidget.h"
// #include "qqcellmain.h"

// 在这两行之上，加入：
#include <QScrollBar>

#include "../../src/utils/StyleLoader.h"


FriendListWidget::FriendListWidget(QWidget *parent)
    : QScrollArea(parent)
{
    // 1. 创建真正的内容容器
    QWidget *container = new QWidget(this);


    // 在 setWidget(container); 之后



    // 2. 在容器上创建主布局
    m_mainLayout = new QVBoxLayout(container);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0,0,0,0);

    // 3. 设置 QScrollArea 属性
    setWidgetResizable(true); // 跟随宽度自适应
    setWidget(container);     // 把容器塞进滚动区


    // 加载样式到 QScrollArea（主要是 viewport、container 的样式）
    StyleLoader::loadWidgetStyle(this, ":/styles/friendlistwidget.css");
    StyleLoader::loadWidgetStyle(verticalScrollBar(), ":/styles/friendlistwidget.css");

}


FriendListWidget::~FriendListWidget() = default;

void FriendListWidget::setGroups(const QList<std::tuple<QString,int,int>>& groups)
{

    // 1. 先清理旧分组：删除所有 titleWidget 和 contentWidget
    for (auto it = m_groups.begin(); it != m_groups.end(); ++it) {
        auto &blk = it.value();
        delete blk.titleWidget;
        delete blk.contentWidget;  // 删除容器，连同其中的子控件和布局都会被一起删除

    }
    m_groups.clear();

    // 2. 重新创建分组
    for (auto &g : groups) {
        QString name; int total, online;
        std::tie(name, total, online) = g;

        auto title = new QQCellTitle(name, this);
        title->setObjectName(name);      // 保存组名，便于后面查找
        title->setOnlineAndGrroup(online, total);
        m_mainLayout->addWidget(title);

        auto container = new QWidget(this);
        auto vlay = new QVBoxLayout(container);
        vlay->setContentsMargins(0,0,0,0);
        vlay->setSpacing(0);
        container->setLayout(vlay);
        container->setVisible(false);
        m_mainLayout->addWidget(container);



        m_groups.insert(name, { title, container, vlay });

        connect(title, &QQCellTitle::sigCellStatusChange,
                this, &FriendListWidget::onGroupToggled);
    }

    m_mainLayout->addStretch();
}

void FriendListWidget::setFriendsForGroup(const QString &groupName, const QList<FriendInfo> &friends)
{
    if (!m_groups.contains(groupName)) return;
    auto &blk = m_groups[groupName];

    // 清空旧好友
    QLayoutItem *child;
    while ((child = blk.contentLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    // 填充新好友
    for (auto &fi : friends) {
        auto cell = new QQCellLine(fi.avatar,fi.displayName(),fi.status,this,fi.friendId);
        // 假设 QQCellMain 有这些接口，你酌情调整
            // cell->setAvatar(fi.avatarUrl);
            // cell->setName(fi.name);
            // cell->setOnline(fi.isOnline);
            // cell->setId(fi.id); // 如果需要

        blk.contentLayout->addWidget(cell);

        // 连接点击信号
        connect(cell, &QQCellLine::clicked, this, &FriendListWidget::onFriendCellClicked);
        // 用 lambda 捕获 fi
        connect(cell, &QQCellLine::clicked, [this,fi](){
            emit friendClicked(fi);
        });
    }
}

void FriendListWidget::onGroupToggled(bool open)
{
    // sender 是哪个 QQCellTitle？
    auto title = qobject_cast<QQCellTitle*>(sender());
    if (!title) return;
    QString name = title->objectName(); // 或者在 QQCellTitle 中加一字段保存 groupName
    if (!m_groups.contains(name)) return;

    auto &blk = m_groups[name];
    blk.contentWidget->setVisible(open);
    title->setOpenState(open);
    emit groupToggled(name, open);
}

void FriendListWidget::onFriendCellClicked()
{
    auto cell = qobject_cast<QQCellLine*>(sender());
    if(!cell)return;

    // 把上一次选中的恢复默认
    if(m_currentSelected &&m_currentSelected !=cell){
        m_currentSelected->setProperty("selected",false);
        m_currentSelected->style()->unpolish(m_currentSelected);
        m_currentSelected->style()->polish(m_currentSelected);
    }

    // ② 给当前这行设置 selected=true
    cell->setProperty("selected", true);
    cell->style()->unpolish(cell);
    cell->style()->polish(cell);


    // 4. 更新指针
    m_currentSelected = cell;

}

