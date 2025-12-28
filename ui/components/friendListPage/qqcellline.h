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
    explicit QQCellLine(const qint64 &uid,const QString &name,bool isOnLine = true,QWidget *parent = nullptr,const int user_id = 0);
    ~QQCellLine();

    // 接口：设置头像（传入本地路径或网络 URL）
    void setAvatar(const qint64 &uid);
    // 接口：设置好友名称
    void setName(const QString &name);
    // 接口：设置在线状态
    void setOnline(bool online);

    // 接口:设置用户ID
    void setUserId(int _id);

    int getUserId();

signals:
    // 点击该好友时发出
    void clicked();

protected:
    // 捕获鼠标点击事件
    void mousePressEvent(QMouseEvent *event) override;


private:
    Ui::QQCellLine *ui;

    int m_user_id;
};

#endif // QQCELLLINE_H
