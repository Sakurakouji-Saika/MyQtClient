#include "app/widget.h"
#include <QtGlobal>
#include <QProcess>
#include "utils/StyleLoader.h"
#include <QApplication>
#include "utils/appconfig.h"
#include "app/mainwindow.h"

#include "DataBaseManage/databasemanage.h"

#include "../../ui/components/Registration_Page/registration_page.h"
#include "../ui/components/addfrienddialog/addfrienddialog.h"

void my_version(){
    QString qtVersion = QT_VERSION_STR;

    QProcess proc;
    proc.start("g++", QStringList() << "--version");
    proc.waitForFinished();
    QString mingwVersion = QString::fromLocal8Bit(proc.readAllStandardOutput()).split("\n").first();

    qDebug().noquote() << QString("Qt version: %1\nMinGW version: %2")
                              .arg(qtVersion, mingwVersion);
}


void test_DBM(){
    // 1. 初始化数据库（accountId, basePath）
    QString accountId = "user_1001";
    // QString basePath = QCoreApplication::applicationDirPath() + "/data";
    QString basePath =  "C:\\Users\\Moe\\Desktop\\MyClient\\data";

    if (!DataBaseManage::instance()->init(accountId, basePath)) {
        qWarning() << "初始化数据库失败";
        return ;
    }


}


// void test2_DBM(){

//     // 2. 添加/更新好友
//     DataBaseManage *mgr = DataBaseManage::instance();


//     bool ok;
//     Add_Friend_Type m_type;

//     m_type = mgr->checkAndAddFriend("friend_2001", "Alice", "/path/to/alice.jpg", 1, "小学同学");
//     switch(m_type){
//     case Add_Friend_Type::exist     :   qDebug() << "该好友已存在"; break;
//     case Add_Friend_Type::success   :   qDebug() << "插入好友成功"; break;
//     case Add_Friend_Type::failure   :   qDebug() << "插入好友失败"; break;
//     }


//     m_type = mgr->checkAndAddFriend("friend_2002", "Bob", "/path/to/bob.png", 0, "同事");

//     switch(m_type){
//     case Add_Friend_Type::exist     :   qDebug() << "该好友已存在"; break;
//     case Add_Friend_Type::success   :   qDebug() << "插入好友成功"; break;
//     case Add_Friend_Type::failure   :   qDebug() << "插入好友失败"; break;
//     }

//     // 3. 添加聊天记录
//     QString msgId1 = QUuid::createUuid().toString();
//     qint64 ts1 = QDateTime::currentSecsSinceEpoch();
//     ok = mgr->addChatMessage(msgId1, "friend_2001", accountId, "Hello Alice!", 0, ts1);
//     qDebug() << "addChatMessage ->" << ok << " msgId:" << msgId1;

//     // 再添加一条来自自己到 friend_2001 的消息
//     QString msgId2 = QUuid::createUuid().toString();
//     qint64 ts2 = QDateTime::currentSecsSinceEpoch() + 1;
//     ok = mgr->addChatMessage(msgId2, accountId, "friend_2001", "Hi, I'm here.", 0, ts2);
//     qDebug() << "addChatMessage ->" << ok << " msgId:" << msgId2;

//     // 4. upsert 最近会话（如果你的 sqlite 支持 ON CONFLICT ... DO UPDATE）
//     ok = mgr->upsertRecentMessage("friend_2001", "Hi, I'm here.", ts2, 0, 0);
//     qDebug() << "upsertRecentMessage ->" << ok;


//     mgr->getFriendList();
//     qDebug() << "----------------------------------";
//     mgr->getRecentMessageList();


//     ok = mgr->insertOrUpdateRecentMessage(
//         "user_12341",
//         "你好！",
//         QDateTime::currentSecsSinceEpoch(),
//         2,
//         1);
//     if(ok){
//         qDebug() << "insertOrUpdateRecentMessage 成功";
//     }




//     // 7. 关闭（可选，程序结束前自动析构也会关闭）
//     mgr->instance()->close();
//     qDebug() << "done";
// }

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qWarning() << "Test warning message";

    test_DBM();



    StyleLoader::setDebugEnabled(true);
    StyleLoader::setDebugResourceRoot("C:/Users/Moe/Desktop/MyClient/src/resources");


    my_version();

    // 一次性加载配置，之后只读
    AppConfig::initialize("://config/app.ini");






    // Widget w;
    // w.show();

    MainWindow m;
    m.show();



    // Registration_Page   m_pg;
    // m_pg.show();

    return a.exec();
}



