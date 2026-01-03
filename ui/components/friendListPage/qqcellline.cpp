#include "qqcellline.h"
#include "ui_qqcellline.h"
#include "../../src/utils/styleLoader.h"
#include "../../src/utils/appconfig.h"
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMouseEvent>
#include "../../src/utils/utils.h"

QQCellLine::QQCellLine(const qint64 &uid,const QString &name, bool isOnLine, QWidget *parent,const int _id)
    : QWidget(parent)
    , ui(new Ui::QQCellLine)
    , m_user_id(_id)
{


    ui->setupUi(this);

    // 打开鼠标跟踪，才能实时接收 hover 事件
    setMouseTracking(true);


    // 打开样式背景支持
    this->setAttribute(Qt::WA_StyledBackground, true);


    // 验证资源文件是否存在
    QString qssPath = ":/styles/qqcellline.css";
    // 加载样式
    StyleLoader::loadWidgetStyle(this,qssPath);

    //自动缩放图片内容
    ui->labHeadeImg->setScaledContents(true);

    setAvatar(uid);

    ui->labFriendName->setText(name);

    if(isOnLine){
        ui->labStatus->setText(QString("[ 在线 🟢 ]"));
    }else{
        ui->labStatus->setText(QString("[ 离线 ]"));
    }
}

QQCellLine::~QQCellLine()
{
    delete ui;
}

void QQCellLine::setAvatar(const qint64 &uid)
{

    qDebug() << "QQCellLine::setAvatar(const qint64 &uid)::uid" <<  uid;

    ui->labHeadeImg->setAvatar(uid,40);
}

void QQCellLine::setName(const QString &name)
{
    ui->labFriendName->setText(name);
}

void QQCellLine::setOnline(bool online)
{
    if(online){
        ui->labStatus->setText(QString("当前用户状态：在线"));
    }else{
        ui->labStatus->setText(QString("当前用户状态：离线"));
    }
}

void QQCellLine::setUserId(int _id)
{
    m_user_id = _id;
}

int QQCellLine::getUserId()
{
    return m_user_id;
}

void QQCellLine::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }

    QWidget::mousePressEvent(event);
}
