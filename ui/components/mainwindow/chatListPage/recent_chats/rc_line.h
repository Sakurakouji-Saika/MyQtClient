#ifndef RC_LINE_H
#define RC_LINE_H

#include <QWidget>
#include "../../Src/utils/utils.h"
#include "../recent_data.h"

namespace Ui {
class RC_Line;
}

class RC_Line : public QWidget
{
    Q_OBJECT

public:
    explicit RC_Line(QWidget *parent = nullptr);
    ~RC_Line();
    void setData(Recent_Data m_data);


private:
    Ui::RC_Line *ui;
};

#endif // RC_LINE_H
