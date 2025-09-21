#ifndef ADDFRIENDDIALOG_H
#define ADDFRIENDDIALOG_H

#include <QWidget>
#include "../../src/utils/StyleLoader.h"


namespace Ui {
class addfrienddialog;
}

class addfrienddialog : public QWidget
{
    Q_OBJECT

public:
    explicit addfrienddialog(QWidget *parent = nullptr);
    ~addfrienddialog();

private:
    Ui::addfrienddialog *ui;
};

#endif // ADDFRIENDDIALOG_H
