#ifndef CHATLIST_MAIN_H
#define CHATLIST_MAIN_H

#include <QWidget>

namespace Ui {
class chatList_Main;
}

class chatList_Main : public QWidget
{
    Q_OBJECT

public:
    explicit chatList_Main(QWidget *parent = nullptr);
    ~chatList_Main();

    void addChatLeft();

private:
    QString m_avatar_url;
    Ui::chatList_Main *ui;
};

#endif // CHATLIST_MAIN_H
