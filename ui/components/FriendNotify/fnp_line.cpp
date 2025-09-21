#include "fnp_line.h"
#include "ui_fnp_line.h"

#include "../../Src/utils/utils.h"
FNP_Line::FNP_Line(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FNP_Line)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_StyledBackground, true);


    StyleLoader::loadWidgetStyle(this, ":/styles/FriendNotify_Page.css");

}

FNP_Line::~FNP_Line()
{
    delete ui;
}

void FNP_Line::setData(const QPixmap &avatar, const QString &qq, const QString &userName, const QString &timeText)
{
    // 确保 QLabel 不自动再额外缩放（避免与我们处理冲突）
    ui->avatarIcon->setScaledContents(false);

    QSize target = ui->avatarIcon->size();

    if (!avatar.isNull() && !target.isEmpty()) {
        // 使用圆形头像：半径为短边的一半；如需圆角而非圆形可改小一点
        int radius = qMin(target.width(), target.height()) / 2;
        QPixmap rounded = scaledRoundedPixmap(avatar, target, radius);
        ui->avatarIcon->setPixmap(rounded);
    } else {
        ui->avatarIcon->clear(); // 或者设置占位图
    }

    ui->qqNumber->setText(qq);
    ui->userName->setText(userName);
    ui->timeText->setText(timeText);
}

void FNP_Line::setData(const QString &avatarPath, const QString &qq, const QString &userName, const QString &timeText)
{
    QPixmap pix;
    if (!avatarPath.isEmpty() && QFile::exists(avatarPath)) {
        pix.load(avatarPath);
    }
    // 调用上面的重载（即便 pix 为空也会清空 avatar）
    setData(pix, qq, userName, timeText);
}
