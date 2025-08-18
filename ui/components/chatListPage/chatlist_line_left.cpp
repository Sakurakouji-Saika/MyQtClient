#include "chatlist_line_left.h"
#include "ui_chatlist_line_left.h"
#include <QtMath>

chatlist_Line_left::chatlist_Line_left(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatlist_Line_left)
{
    ui->setupUi(this);

    ui->horizontalLayout->setAlignment(ui->textBrowser, Qt::AlignTop | Qt::AlignLeft);


    int num1 = 100 ;


    ui->textBrowser->setText("111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111\n122222222222222222222222222222222");
    adjustTextBrowserToHostWidthNoMax(ui->textBrowser, this);





    // ui->textBrowser->setMinimumHeight(num1);
    // ui->textBrowser->setMaximumHeight(num1);
    this->setMinimumHeight(num1);
    this->setMaximumHeight(num1);


}

chatlist_Line_left::~chatlist_Line_left()
{
    delete ui;
}

void chatlist_Line_left::adjustTextBrowserToHostWidthNoMax(QTextBrowser *tb, QWidget *host, int margin)
{
    if (!tb || !host) return;

    // 用临时文档测量内容（不影响真实 document 的 textWidth）
    QString html = tb->toHtml();
    QTextDocument tmpDoc;
    tmpDoc.setDefaultFont(tb->font());
    tmpDoc.setHtml(html);

    // 先不限制宽度，测出原始所需宽度（不换行情况下）
    tmpDoc.setTextWidth(-1);
    tmpDoc.adjustSize();
    QSizeF origSize = tmpDoc.size();

    int frame = tb->frameWidth() * 2;
    int hostW = host->width();

    // 计算期望宽度（内容 + 边框 + margin）
    int desiredW = qCeil(origSize.width()) + frame + margin;

    // 若期望宽度超过 host 宽度，则用 host 宽度并重新以该宽度换行计算高度
    if (desiredW > hostW) {
        int contentW = hostW - frame - margin;
        if (contentW < 10) contentW = 10;
        tmpDoc.setTextWidth(contentW);
        tmpDoc.adjustSize();
        desiredW = hostW;
    }

    // 高度按内容完整计算（**无上限**）
    int desiredH = qCeil(tmpDoc.size().height()) + frame + margin;

    // 滚动策略：既然高度不受限，通常不需要滚动条（保持水平关闭）
    tb->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tb->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 设置尺寸（若在 layout 中需要被 layout 管理，可改为 setMaximumSize / setMinimumSize）
    tb->setFixedSize(desiredW, desiredH);
}
