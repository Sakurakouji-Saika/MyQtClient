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

    // Add example messages
    for (int i = 0; i < 10; ++i) {
         chat->addMessage(false,"D:\\Documents\\Qt\\MyClient\\src\\resources\\picture\\avatar\\3.jpg","AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA123");
         chat->addMessage(false,"D:\\Documents\\Qt\\MyClient\\src\\resources\\picture\\avatar\\4.jpg","对方消息：短文本会按文本长度显示，不会被强制换行（若能单行完整显示则不换行）。");
         chat->addMessage(true,"://picture/avatar/2.jpg","我的消息：短消息也是单行显示且靠右。");
         chat->addMessage(false,"://picture/avatar/1.jpg","我的消息：短消息也是单行显示且靠右。");

         chat->addMessage(false,"://picture/avatar/1.jpg","这是一条非常长的对方消息示例，用来测试当文本宽度超过可用宽度时，气泡会被拉满并自动换行。窗口缩放时，气泡会根据 scroll 区域的可用宽度重新设置宽度，从而触发文本换行和高度自适应。");
    }


    chat->addMessage(false,"://picture/avatar/1.jpg","Moe 可爱捏！");
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setWidget(chat);

    old->setContentsMargins(0,0,0,0);
    // ---------- end ----------

    connect(ui->btn_pushMsg,&QToolButton::clicked,this,[this,chat]{
        QString t = ui->CLM_plainTextEdit->toPlainText().trimmed();
        if(!t.isEmpty()){
            chat->addMessage(true,"://picture/avatar/1.jpg",t);
            ui->CLM_plainTextEdit->clear();
        }
    });


}

chatList_Main::~chatList_Main()
{
    delete ui;
}
