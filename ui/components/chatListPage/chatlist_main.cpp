#include "chatlist_main.h"
#include "ui_chatlist_main.h"
#include "../../src/utils/StyleLoader.h"
#include "chatlist_line_left.h"


chatList_Main::chatList_Main(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatList_Main)
{
    ui->setupUi(this);

    ui->splitter->setSizes({400, 100});

    // 验证资源文件是否存在
    QString qssPath = ":/styles/chatListPage.css";
    // 加载样式
    StyleLoader::loadWidgetStyle(this,qssPath);

    chatlist_Line_left *clll = new chatlist_Line_left(this);

    clll->setParent(ui->scrollAreaWidgetContents);

    // 2. 确保内容区有 QVBoxLayout
    QVBoxLayout *vlayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    if (!vlayout) {
        vlayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
        vlayout->setContentsMargins(6,6,6,6);
        vlayout->setSpacing(8);
        // 若希望消息靠上，添加 stretch
        vlayout->addStretch();
    }

    // 3. 让子控件横向扩展填满宽度
    clll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    // 4. 插入到 stretch 之前（如果有 stretch）
    int insertIndex = vlayout->count();
    if (insertIndex > 0 && vlayout->itemAt(insertIndex - 1)->spacerItem()) {
        insertIndex = insertIndex - 1;
    }
    vlayout->insertWidget(insertIndex, clll);

    // 5. 刷新并滚到底
    ui->scrollAreaWidgetContents->adjustSize();
    QScrollBar *vbar = ui->scrollArea->verticalScrollBar();
    // vbar->setValue(vbar->maximum());

}

chatList_Main::~chatList_Main()
{
    delete ui;
}
