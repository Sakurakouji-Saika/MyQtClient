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

    // 2. 添加/更新好友
    DataBaseManage *mgr = DataBaseManage::instance();


    bool ok;

    if(mgr->isFriend("friend_2002")){
        qDebug()<< "已经是好友了";
    }else{
        qDebug()<< "不是是好友了";
        ok = mgr->addFriend("friend_2001", "Alice", "/path/to/alice.jpg", 1, "小学同学");
        qDebug() << "addFriend Alice ->" << ok;
    }


    if(mgr->isFriend("friend_2002")){
        qDebug()<< "已经是好友了";
    }else{
        qDebug()<< "不是是好友了";
        mgr->addFriend("friend_2002", "Bob", "/path/to/bob.png", 0, "同事");
    }

    // 3. 添加聊天记录
    QString msgId1 = QUuid::createUuid().toString();
    qint64 ts1 = QDateTime::currentSecsSinceEpoch();
    ok = mgr->addChatMessage(msgId1, "friend_2001", accountId, "Hello Alice!", 0, ts1);
    qDebug() << "addChatMessage ->" << ok << " msgId:" << msgId1;

    // 再添加一条来自自己到 friend_2001 的消息
    QString msgId2 = QUuid::createUuid().toString();
    qint64 ts2 = QDateTime::currentSecsSinceEpoch() + 1;
    ok = mgr->addChatMessage(msgId2, accountId, "friend_2001", "Hi, I'm here.", 0, ts2);
    qDebug() << "addChatMessage ->" << ok << " msgId:" << msgId2;

    // 4. upsert 最近会话（如果你的 sqlite 支持 ON CONFLICT ... DO UPDATE）
    ok = mgr->upsertRecentMessage("friend_2001", "Hi, I'm here.", ts2, 0, 0);
    qDebug() << "upsertRecentMessage ->" << ok;


}

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






    Widget w;
    w.show();

    // MainWindow m;
    // m.show();



    // Registration_Page   m_pg;
    // m_pg.show();

    return a.exec();
}



