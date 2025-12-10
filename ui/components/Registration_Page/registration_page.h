#ifndef REGISTRATION_PAGE_H
#define REGISTRATION_PAGE_H

#include <QWidget>


#include "../../Src/Network/Service/authservice.h"
#include "../../Src/Network/Service/servicemanager.h"
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
    void setNetwork(ServiceManager *_sm);

private slots:
    void on_pushButton_clicked();

private:
    Ui::Registration_Page *ui;

    ServiceManager *m_sm;
};

#endif // REGISTRATION_PAGE_H
