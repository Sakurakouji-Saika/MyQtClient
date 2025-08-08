#ifndef QQCELLLINE_H
#define QQCELLLINE_H

#include <QWidget>

namespace Ui {
class QQCellLine;
}

class QQCellLine : public QWidget
{
    Q_OBJECT

public:
    explicit QQCellLine(const QString &avatarUrl,const QString &name,bool isOnLine = true,QWidget *parent = nullptr);
    ~QQCellLine();

    // 接口：设置头像（传入本地路径或网络 URL）
    void setAvatar(const QString &avatarUrl);
    // 接口：设置好友名称
    void setName(const QString &name);
    // 接口：设置在线状态
    void setOnline(bool online);

signals:
    // 点击该好友时发出
    void clicked();

protected:
    // 捕获鼠标点击事件
    void mousePressEvent(QMouseEvent *event) override;


private:
    Ui::QQCellLine *ui;
};

#endif // QQCELLLINE_H
