#ifndef QQCELLTITLE_H
#define QQCELLTITLE_H

#include <QWidget>
#include <QPixmap>

namespace Ui {
class QQCellTitle;
}

class QQCellTitle : public QWidget
{
    Q_OBJECT

public:
    explicit QQCellTitle(const QString &groupName,QWidget *parent = nullptr);
    ~QQCellTitle();

    void setOnlineAndGrroup(int online = 0,int groupCount = 0);

    void setOpenState(bool);

signals:
    void sigCellStatusChange(bool);

private:
    Ui::QQCellTitle *ui;

    bool m_isOpen = false;

    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};



#endif // QQCELLTITLE_H
