#ifndef FNP_LINE_H
#define FNP_LINE_H

#include <QWidget>
#include <QFile>

#include "FNPData.h"
#include "../../src/utils/styleLoader.h"

namespace Ui {
class FNP_Line;
}

class FNP_Line : public QWidget
{
    Q_OBJECT

public:
    explicit FNP_Line(QWidget *parent = nullptr);
    ~FNP_Line();

    // 使用图片路径设置（若加载失败，会清空 avatar）
    void setData(const QString &str_uid,
                 const QString &userName,
                 const QString &timeText,
                 const qint64 &_uid
                );

private:
    Ui::FNP_Line *ui;
    qint64 m_uid;
};

#endif // FNP_LINE_H
