#ifndef PLACEHOLDERPAGE_H
#define PLACEHOLDERPAGE_H

#include <QWidget>

namespace Ui {
class placeholderPage;
}

class placeholderPage : public QWidget
{
    Q_OBJECT

public:
    explicit placeholderPage(QWidget *parent = nullptr);
    ~placeholderPage();

private:
    Ui::placeholderPage *ui;
};

#endif // PLACEHOLDERPAGE_H
