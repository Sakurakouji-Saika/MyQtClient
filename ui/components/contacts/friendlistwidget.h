// FriendListWidget.h
#ifndef FRIENDLISTWIDGET_H
#define FRIENDLISTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QMap>
#include "QQCellTitle.h"
#include "FriendInfo.h"
#include <QScrollArea>
#include <QVariant>
#include <QStyle>     // 为 style()->unpolish()/polish() 提供完整定义
#include "qqcellline.h"

class FriendListWidget : public QScrollArea {
    Q_OBJECT
public:
    explicit FriendListWidget(QWidget *parent = nullptr);
    ~FriendListWidget() override;

    // 接口：一次性设置所有分组（仅标题），并给出人数&在线数
    void setGroups(const QList<std::tuple<QString,int,int>>& groups);
    // 接口：为指定分组填充好友列表
    void setFriendsForGroup(const QString &groupName, const QList<FriendInfo> &friends);

signals:
    void friendClicked(const FriendInfo &info);
    void groupToggled(const QString &groupName, bool expanded);

private:
    struct GroupBlock {
        QQCellTitle*        titleWidget;
        QWidget*            contentWidget;
        QVBoxLayout*        contentLayout;
    };

    QWidget    *m_container;    // 真正放标题和列表的 QWidget
    QVBoxLayout*           m_mainLayout;    // 放到 container 里的主布局
    QMap<QString,GroupBlock> m_groups;


    QQCellLine* m_currentSelected = nullptr;    //当前选中的好友指针

private slots:
    void onGroupToggled(bool open);
    void onFriendCellClicked();
};

#endif // FRIENDLISTWIDGET_H
