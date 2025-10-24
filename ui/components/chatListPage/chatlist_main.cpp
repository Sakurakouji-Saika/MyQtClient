#include "chatlist_main.h"
#include "ui_chatlist_main.h"
#include "../../src/utils/StyleLoader.h"
#include <QUuid>

#include <QVBoxLayout>



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
    chat = new ChatWidget;
    // 可以根据需要设置滚动区域是否自动调整子 widget 的大小

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setWidget(chat);

    old->setContentsMargins(0,0,0,0);
    // ---------- end ----------


}

chatList_Main::~chatList_Main()
{
    delete ui;
}

void chatList_Main::openChatPage(const QString _id)
{
    qDebug ()<< "chatList_Main::openChatPage(const QString _id)::触发::打开ID为：" << _id << "|| 本地用户为：" << AppConfig::UserID();

    m_user_name = DataBaseManage::instance()->getDisplayNameByFriendId(_id);
    m_user_id = _id;

    ui->labelContactNickname->setText(m_user_name);

    QList<ChatRecord> m_list = DataBaseManage::instance()->getChatRecords(AppConfig::UserID(),_id);
    QString m_avatar_url;
    for(int i=0;i<m_list.size();i++){

        if(AppConfig::UserID() == m_list[i].fromId ){
            m_avatar_url = DataBaseManage::instance()->getAvatarByFriendId(AppConfig::UserID());

            addChatLeft(true,m_avatar_url,m_list[i].content);
        }else{
            m_avatar_url =DataBaseManage::instance()->getAvatarByFriendId(_id);

            addChatLeft(false,m_avatar_url,m_list[i].content);
        }
        qDebug() << "chatList_Main::openChatPage::" <<m_list[i].content;
    }

}

void chatList_Main::addChatLeft(bool isMy, const QString avatar, const QString msg)
{
    chat->addMessage(isMy,avatar,msg);
}


void chatList_Main::MsgALLClear()
{
    chat->clearAllMsg();
}

void chatList_Main::on_btn_pushMsg_clicked()
{
    QString t = ui->CLM_plainTextEdit->toPlainText().trimmed();
    if (t.isEmpty()) return;

    // 防止重复发送：临时禁用发送按钮
    ui->btn_pushMsg->setEnabled(false);

    const QString msgId = QUuid::createUuid().toString();
    const qint64 ts = QDateTime::currentSecsSinceEpoch();

    // 原子地写入聊天记录并更新最近会话（事务内）
    bool ok = DataBaseManage::instance()->addChatMessageAndUpdateRecent(
                msgId,
                AppConfig::UserID(),
                m_user_id,
                t,
                0,
                ts,
                m_user_id,
                t,
                ts,
                0,
                1  // direction: 1 表示 outgoing（发送）
    );

    if (ok) {
        // 只有在数据库写入成功后再更新 UI，确保 UI 与持久层一致
        chat->addMessage(true, DataBaseManage::instance()->getAvatarByFriendId(AppConfig::UserID()), t);
        ui->CLM_plainTextEdit->clear();

        ChatRecord cr;
        cr.msgId = msgId;
        cr.content = t;
        cr.fromId = AppConfig::UserID();
        cr.toId = m_user_id;
        cr.timestamp = ts;

        emit SeedMsg(cr);
        qDebug() << "chatlist_main: emitted SeedMsg -> peer:" << cr.toId << " msg:" << cr.content;
    } else {
        // QMessageBox::warning(this, tr("发送失败"), tr("消息发送失败，请检查网络或重试。"));
        qDebug() << "chatlist_main: addChatMessageAndUpdateRecent failed for peer:" << m_user_id << " msg:" << t;
    }

    ui->btn_pushMsg->setEnabled(true);
}

