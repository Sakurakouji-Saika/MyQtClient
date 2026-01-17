#ifndef RC_LINE_H
#define RC_LINE_H

#include <QWidget>
#include "../recent_time_utlis.h"
#include "../../src/utils/utils.h"
#include "../recent_data.h"
#include <QDateTime>
#include <QDate>
#include <QTime>

namespace Ui {
class RC_Line;
}

class RC_Line : public QWidget
{
    Q_OBJECT

public:
    explicit RC_Line(QWidget *parent = nullptr);
    ~RC_Line();

    void setData(const Recent_Data &m_data);
    void setData(QVariant m_var);
    void setUnReadOnZero();
    Recent_Data recentDataFromVariant(const QVariant &v);

    qint64 getUid();


private:
    Ui::RC_Line *ui;
    qint64 m_uid;
};

#endif // RC_LINE_H
