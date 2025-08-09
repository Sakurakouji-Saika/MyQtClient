#include "qqcelltitle.h"
#include "ui_qqcelltitle.h"
#include <QEvent>
#include <QMouseEvent>
#include "../../Src/utils/StyleLoader.h"

QQCellTitle::QQCellTitle(const QString &groupName, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QQCellTitle)
{
    ui->setupUi(this);


    // 打开样式背景支持
    this->setAttribute(Qt::WA_StyledBackground, true);

    // ui->horizontalLayout->setAlignment(Qt::AlignVCenter);

    // 验证资源文件是否存在
    QString qssPath = ":/styles/qqcelltitle.css";
    // 加载样式
    StyleLoader::loadWidgetStyle(this,qssPath);



    ui->labGroupName->setText(groupName);



    // 安装事件过滤器
    installEventFilter(this);
    setAttribute(Qt::WA_Hover, true);

    // 初始状态
    m_isOpen = false;
}

QQCellTitle::~QQCellTitle()
{
    delete ui;
}

void QQCellTitle::setOnlineAndGrroup(int online, int groupCount)
{
    ui->labGroupNumber->setText(QString("[%1/%2]").arg(online).arg(groupCount));
}

bool QQCellTitle::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this) {
        if (event->type() == QEvent::HoverEnter) {
            // // 悬停效果
            // setStyleSheet("background-color: #E0E0E0;");
            // return true;
            return QWidget::eventFilter(watched, event);

        } else if (event->type() == QEvent::HoverLeave) {
            // setStyleSheet("background-color: none;");
            // return true;
            return QWidget::eventFilter(watched, event);

        } else if (event->type() == QEvent::MouseButtonPress) {
            // 发送状态改变信号
            m_isOpen =!m_isOpen;
            emit sigCellStatusChange(m_isOpen);
            return true;


        }
    }
    return QWidget::eventFilter(watched, event);
}

void QQCellTitle::setOpenState(bool open)
{
    m_isOpen = open;
    // 可以在这里添加箭头图标变化等

    if(m_isOpen){
        ui->label->setPixmap(QPixmap("://svg/chevron_down_new_16.svg"));
    }else{
        ui->label->setPixmap(QPixmap("://svg/chevron_right_new_16.svg"));
    }
}
