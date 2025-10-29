#include "app/widget.h"
#include <QtGlobal>
#include <QProcess>
#include "utils/StyleLoader.h"
#include <QApplication>
#include "utils/appconfig.h"
#include "app/mainwindow.h"

#include "DataBaseManage/databasemanage.h"
#include "../src/Network/networkadapter.h"


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


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qWarning() << "Test warning message";

    test_DBM();




    StyleLoader::setDebugEnabled(true);
    StyleLoader::setDebugResourceRoot("C:/Users/Moe/Desktop/MyClient/src/resources");

    NetworkAdapter *netAdapter = new NetworkAdapter(nullptr, qApp); // qApp 为 parent，统一管理

    my_version();

    AppConfig::instance().initialize();


    // Widget w(netAdapter); // 注入
    // w.show();


    MainWindow m;
    m.SetNetwork(netAdapter);
    m.show();


    return a.exec();
}



