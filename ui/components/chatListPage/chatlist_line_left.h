#ifndef CHATLIST_LINE_LEFT_H
#define CHATLIST_LINE_LEFT_H

#include <QWidget>
#include <QTextBrowser>

namespace Ui {
class chatlist_Line_left;
}

class chatlist_Line_left : public QWidget
{
    Q_OBJECT

public:
    explicit chatlist_Line_left(QWidget *parent = nullptr);
    ~chatlist_Line_left();


    void adjustTextBrowserToHostWidthNoMax(QTextBrowser *tb, QWidget *host, int margin = 8);

private:
    Ui::chatlist_Line_left *ui;
};

#endif // CHATLIST_LINE_LEFT_H
