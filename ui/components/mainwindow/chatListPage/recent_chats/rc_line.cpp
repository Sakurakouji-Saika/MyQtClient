#include "rc_line.h"
#include "ui_rc_line.h"
#include <qDebug>
#include <QStyle>

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

void RC_Line::setData(Recent_Data m_data)
{
    // 设置头像
    QSize avatarSize;
    avatarSize.setWidth(40);
    avatarSize.setHeight(40);
    ui->RCL_Avatar->setPixmap(scaledRoundedPixmap(QPixmap(m_data.avatarPath),avatarSize,40));

    // 设置其他属性
    ui->RCL_LastMsgTime->setText(m_data.timeText);
    ui->RCL_UserName->setText(m_data.userName);

    QString s = ui->RCL_MessagePreview->fontMetrics().elidedText(m_data.msg,Qt::ElideRight,ui->RCL_MessagePreview->width());
    ui->RCL_MessagePreview->setText(s);
    // ui->RCL_MessagePreview->adjustSize();





    // 设置未读数
    int unread = m_data.UnreadCount;
    qDebug() << "unread:" << unread;

    bool hasUnread = unread > 0;
    ui->RCL_UnreadCount->setText(hasUnread ? QString::number(unread) : "");
    ui->RCL_UnreadCount->setVisible(hasUnread);

    // 切换样式类
    if (hasUnread) {
        ui->groupBox->setProperty("class", "unread");
    } else {
        ui->groupBox->setProperty("class", "no-unread");
    }





    // 强制刷新样式
    ui->groupBox->style()->unpolish(ui->groupBox);
    ui->groupBox->style()->polish(ui->groupBox);
    ui->groupBox->update();



}
