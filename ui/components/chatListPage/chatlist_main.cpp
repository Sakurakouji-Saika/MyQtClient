#include "chatlist_main.h"
#include "ui_chatlist_main.h"
#include "../../src/utils/StyleLoader.h"

#include <QVBoxLayout>
#include "chat-widget/chatwidget.h"



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

    // ---------- 把 ChatWidget 放到 scrollArea ----------
    // 先把 Designer 放进去的占位 widget 取出来并删除（可选但推荐）
    QWidget *old = ui->scrollArea->takeWidget();
    if (old) {
        old->deleteLater();
    }

    // 创建你的 ChatWidget（父对象交给 scrollArea）
    ChatWidget *chat = new ChatWidget;
    // 可以根据需要设置滚动区域是否自动调整子 widget 的大小
    chat->addMessage("Moe 可爱捏！",true);
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setWidget(chat);
    // ---------- end ----------

    connect(ui->btn_pushMsg,&QToolButton::clicked,this,[this,chat]{
        QString t = ui->plainTextEdit->toPlainText().trimmed();
        if(!t.isEmpty()){
            chat->addMessage(t,true);
            ui->plainTextEdit->clear();
        }
    });


}

chatList_Main::~chatList_Main()
{
    delete ui;
}
