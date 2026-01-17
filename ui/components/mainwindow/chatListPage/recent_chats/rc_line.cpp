#include "rc_line.h"
#include "ui_rc_line.h"
#include <QDebug>

#include <QStyle>
#include <QDir>
#include "../../src/utils/appconfig.h"

RC_Line::RC_Line(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RC_Line)
{
    ui->setupUi(this);
}

RC_Line::~RC_Line()
{
    delete ui;
}

void RC_Line::setData(const Recent_Data &m_data)
{
    // 设置头像


    QSize avatarSize;
    avatarSize.setWidth(40);
    avatarSize.setHeight(40);

    qDebug() << "RC_Line::setData::m_data.user_id::" << m_data.user_id;

    m_uid = m_data.user_id;


    ui->RCL_Avatar->setAvatar(m_data.user_id,40);



    // 设置其他属性
    ui->RCL_UserName->setText(m_data.userName);

    QString s = ui->RCL_MessagePreview->fontMetrics().elidedText(m_data.msg,Qt::ElideRight,ui->RCL_MessagePreview->width());
    ui->RCL_MessagePreview->setText(s);

    // 设置未读数
    int unread = m_data.UnreadCount;

    bool hasUnread = unread > 0;
    ui->RCL_UnreadCount->setText(hasUnread ? QString::number(unread) : "");
    ui->RCL_UnreadCount->setVisible(hasUnread);

    // 切换样式类
    if (hasUnread) {
        ui->groupBox->setProperty("class", "unread");
    } else {
        ui->groupBox->setProperty("class", "no-unread");
    }

    // 设置现实时间
    ui->RCL_LastMsgTime->setText(formatMessageTimeSmart(m_data.msg_time));


    // 强制刷新样式
    ui->groupBox->style()->unpolish(ui->groupBox);
    ui->groupBox->style()->polish(ui->groupBox);
    ui->groupBox->update();
}

void RC_Line::setData(QVariant m_var)
{
    Recent_Data m = recentDataFromVariant(m_var);
    setData(m);
}

void RC_Line::setUnReadOnZero()
{
    ui->RCL_UnreadCount->setText("");
    ui->groupBox->setVisible(false);
    ui->groupBox->setProperty("class", "no-unread");
}

Recent_Data RC_Line::recentDataFromVariant(const QVariant &v)
{
    Recent_Data rd;

    if (!v.isValid())
        return rd;

    if (v.canConvert<Recent_Data>()) {
        rd = v.value<Recent_Data>();
        return rd;
    }

    if (v.canConvert<QVariantMap>()) {
        QVariantMap m = v.toMap();
        rd.avatarPath  = m.value("avatarPath").toString();
        rd.msg         = m.value("msg").toString();
        rd.user_id     = m.value("user_id").toInt();
        rd.userName    = m.value("userName").toString();
        rd.msg_time    = m.value("msg_time").toDateTime();
        rd.timestamp   = m.value("timestamp").toLongLong();
        rd.UnreadCount = m.value("unreadCount").toInt();
        return rd;
    }

    if (v.type() == QVariant::Map) { // 兼容旧 Qt
        QVariantMap m = v.toMap();
        rd.avatarPath  = m.value("avatarPath").toString();
        rd.msg         = m.value("msg").toString();
        rd.user_id     = m.value("user_id").toInt();
        rd.userName    = m.value("userName").toString();
        rd.msg_time    = m.value("msg_time").toDateTime();
        rd.timestamp   = m.value("timestamp").toLongLong();
        rd.UnreadCount = m.value("UnreadCount").toInt();
        return rd;
    }

    return rd;
}

qint64 RC_Line::getUid()
{
    return m_uid;
}
