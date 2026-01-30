#include "chatlist_main.h"
#include "ui_chatlist_main.h"
#include "../../src/utils/styleLoader.h"
#include <QUuid>
#include <QDir>
#include <QVBoxLayout>
#include <QThreadPool>
#include <QMetaObject>
#include "../../../src/Network/Service/servicemanager.h"
#include "../../../src/Network/Service/friendservice.h"


chatList_Main::chatList_Main(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatList_Main)
{
    ui->setupUi(this);

    ui->splitter->setSizes({400, 100});

    QString qssPath = ":/styles/chatListPage.css";
    StyleLoader::loadWidgetStyle(this,qssPath);

    QWidget *old = ui->scrollArea->takeWidget();
    if (old) {
        old->deleteLater();
    }

    chat = new ChatWidget;

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setWidget(chat);

    old->setContentsMargins(0,0,0,0);
}

chatList_Main::~chatList_Main()
{
    delete ui;
}

void chatList_Main::openChatPage(const int _id)
{

    if(_id == -99){
        Test_Data_Display(AppConfig::instance().getNumberBubbles());
    }


    // 1.打开聊天对话框就要清理最近聊天未读记录，同时发送网络请求表示 已读

    DataBaseManage::instance()->setUnreadCountToZeroByUid(_id);

    m_sm->friendApi()->SendMessageReadReceipt(_id,AppConfig::instance().getUserID());


    // 2.打开聊天对话框就要清理最近聊天未读记录，同时发送网络请求表示 已读



    m_user_name = DataBaseManage::instance()->getDisplayNameByFriendId(_id);
    m_user_id = _id;

    ui->labelContactNickname->setText(m_user_name);

    QList<ChatRecord> m_list = DataBaseManage::instance()->getChatRecords(AppConfig::instance().getUserID(),_id);

    auto safeAvatarPath = [&](qint64 uid)->QString {

        auto opt = DataBaseManage::instance()->GetFriendAvatarById(uid);

        if (opt.has_value() && !opt->avatar.isEmpty()) {
            return AppConfig::instance().imagesDirectory() + QDir::separator() + opt->avatar;
        }

        qDebug();
        return QString();

    };

    for (int i = 0; i < m_list.size(); ++i) {
        if (AppConfig::instance().getUserID() == m_list[i].fromId) {

            QString m_avatar_url = safeAvatarPath(AppConfig::instance().getUserID());
            addChatLeft(true, m_avatar_url, m_list[i].content);
            qDebug();

        } else {
            QString m_avatar_url = safeAvatarPath(_id);
            addChatLeft(false, m_avatar_url, m_list[i].content);
        }
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

void chatList_Main::SetNetWork(ServiceManager *_sm)
{
    m_sm = _sm;
}

void chatList_Main::Test_Data_Display(const int quantity)
{

    auto safeAvatarPath = [&](qint64 uid)->QString {

        auto opt = DataBaseManage::instance()->GetFriendAvatarById(uid);

        if (opt.has_value() && !opt->avatar.isEmpty()) {
            return AppConfig::instance().imagesDirectory() + QDir::separator() + opt->avatar;
        }

        qDebug();
        return QString();

    };


    QString my_avatar = safeAvatarPath(AppConfig::instance().getUserID());


    QThreadPool::globalInstance() -> start([this, my_avatar, quantity]() {

        QList < MessageData > m_messages;
        for (size_t i = 0; i < quantity; i++) {
            MessageData item;
            item.avatar_url = my_avatar;
            item.isSelf = true;
            item.text = "测试一万条数据::" + QString::number(i);
            m_messages.append(item);
        }


        int chunk = 500;
        for (int i = 0; i < m_messages.size(); i += chunk) {
            QList<MessageData> slice = m_messages.mid(i, chunk);
            QMetaObject::invokeMethod(this,
                                      [this, batch = std::move(slice)]() mutable {
                                            chat -> addMessages(batch);
                                      },
                                      Qt::QueuedConnection);
            QThread::msleep(200); // 或用 QTimer 分帧插入，避免阻塞主线程太久
        }


    });



}

void chatList_Main::on_btn_pushMsg_clicked()
{
    QString textContent = ui->CLM_plainTextEdit->toPlainText().trimmed();
    if (textContent.isEmpty()) return;

    // 防止重复发送：临时禁用发送按钮
    ui->btn_pushMsg->setEnabled(false);

    const qint64 ts = QDateTime::currentSecsSinceEpoch();


    // 网络发送
    m_sm->friendApi()->SendMessage(AppConfig::instance().getUserID(),m_user_id,1,textContent,0,ts);

    const QString msgId = QUuid::createUuid().toString();

    // 原子地写入聊天记录并更新最近会话（事务内）
    bool ok = DataBaseManage::instance()->addChatMessageAndUpdateRecent(
                msgId,
                AppConfig::instance().getUserID(),
                m_user_id,
                textContent,
                0,
                ts,
                m_user_id,
                textContent,
                ts,
                0,
                1  // direction: 1 表示 outgoing（发送）
    );

    if (ok) {
        // 只有在数据库写入成功后再更新 UI，确保 UI 与持久层一致
        QString senderAvatar = [&]{
            auto opt = DataBaseManage::instance()->GetFriendAvatarById(AppConfig::instance().getUserID());
            if (opt.has_value() && !opt->avatar.isEmpty()) return AppConfig::instance().imagesDirectory() + QDir::separator() + opt->avatar;
            return QString();
        }();
        chat->addMessage(true, senderAvatar, textContent);
        ui->CLM_plainTextEdit->clear();

        ChatRecord cr;
        cr.msgId = msgId;
        cr.content = textContent;
        cr.fromId = AppConfig::instance().getUserID();
        cr.toId = m_user_id;
        cr.timestamp = ts;

        emit MY_SeedMsg(cr);
        qDebug() << "chatlist_main: emitted SeedMsg -> peer:" << cr.toId << " msg:" << cr.content;

    } else {
        qDebug() << "chatlist_main: addChatMessageAndUpdateRecent failed for peer:" << m_user_id << " msg:" << textContent;
    }

    ui->btn_pushMsg->setEnabled(true);
}

