#ifndef CHATCONTENTPAGE_H
#define CHATCONTENTPAGE_H

#include <QWidget>

namespace Ui {
class chatContentPage;
}

class chatContentPage : public QWidget
{
    Q_OBJECT

public:
    explicit chatContentPage(QWidget *parent = nullptr);
    ~chatContentPage();

private:
    Ui::chatContentPage *ui;
};

#endif // CHATCONTENTPAGE_H
