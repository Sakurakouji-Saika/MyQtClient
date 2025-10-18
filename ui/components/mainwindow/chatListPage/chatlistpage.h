#ifndef CHATLISTPAGE_H
#define CHATLISTPAGE_H

#include <QWidget>
#include "../../src/utils/StyleLoader.h"
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
    void populateRecentList(const QMap<QString,Recent_Data> &recentList);
    void on_showListContextMenu(const QPoint &pos);

    void receiveMessage(const Recent_Data &msg);



private slots:
    void onListItemClicked(const QModelIndex &index);
    void onNewMessage(const Recent_Data &msg);


private:
    Ui::chatListPage *ui;

    Model *m_model;
    QMenu *menu;
};

#endif // CHATLISTPAGE_H
