#ifndef CHATLISTPAGE_H
#define CHATLISTPAGE_H

#include <QWidget>
#include "../../src/utils/StyleLoader.h"
#include "recent_data.h"
#include <QMap>
#include <QMenu>
#include <QGraphicsDropShadowEffect>

#include <QListWidgetItem>

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

    void showListContextMenu(const QPoint &pos);    //显示右键菜单
    void populateRecentList(const QMap<QString,Recent_Data>& recentList);

    // 公共接口，MainWindow 调用它即可（线程安全/非线程安全由你选择）
    void receiveMessage(const Recent_Data &msg);

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

    void addRecent(const Recent_Data &r, bool toTop, bool select);
    void onNewMessage(const Recent_Data &msg);

private:
    Ui::chatListPage *ui;
    QMenu *menu;
    QMap<QString,Recent_Data> m_list;
};

#endif // CHATLISTPAGE_H
