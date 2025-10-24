#ifndef CHATLISTPAGE_H
#define CHATLISTPAGE_H

#include <QWidget>
#include "../../src/utils/StyleLoader.h"

#include "../../Src/utils/AutoHideScrollbar/autohidescrollbar.h"
#include "../../Src/DataBaseManage/model/ChatRecord.h"

#include "recent_data.h"
#include <QMap>
#include <QMenu>
#include <QGraphicsDropShadowEffect>
#include "model.h"
#include <QListView>
#include <QClipboard>
#include <QListWidgetItem>
#include <QStandardItemModel>

namespace Ui {
class chatListPage;
}

class chatListPage : public QWidget
{
    Q_OBJECT

public:
    explicit chatListPage(QWidget *parent = nullptr);
    ~chatListPage();

    void test();
    void populateRecentList(const QList<Recent_Data> &recentList);
    void on_showListContextMenu(const QPoint &pos);

    void receiveMessage(const Recent_Data &msg);


signals:
    void openChatPage(const QString &user_id); // 自定义信号

private slots:
    void onListItemClicked(const QModelIndex &index);
    void onNewMessage(const Recent_Data &msg);
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);


private:
    Ui::chatListPage *ui;

    Model *m_model;
    QMenu *menu;
};

#endif // CHATLISTPAGE_H
