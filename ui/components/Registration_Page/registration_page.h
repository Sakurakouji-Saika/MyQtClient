#ifndef REGISTRATION_PAGE_H
#define REGISTRATION_PAGE_H

#include <QWidget>

#include "../../src/utils/StyleLoader.h"

namespace Ui {
class Registration_Page;
}

class Registration_Page : public QWidget
{
    Q_OBJECT

public:
    explicit Registration_Page(QWidget *parent = nullptr);
    ~Registration_Page();

private:
    Ui::Registration_Page *ui;
};

#endif // REGISTRATION_PAGE_H
