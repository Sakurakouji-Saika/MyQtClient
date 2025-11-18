#include "app/widget.h"
#include <QtGlobal>
#include <QProcess>
#include "utils/StyleLoader.h"
#include <QApplication>
#include "utils/appconfig.h"
#include "app/mainwindow.h"

#include "DataBaseManage/databasemanage.h"


class Widget;

void test(){

}



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qWarning() << "Test warning message";


    StyleLoader::setDebugEnabled(false);
    StyleLoader::setDebugResourceRoot("C:/Users/Moe/Desktop/MyClient/src/resources");



    AppConfig::instance().initialize();

    AppConfig::instance().setUserID(3);

    // AvatarManagerTest tester;
    // tester.startTest();


    Widget w; // 注入
    w.show();


    // MainWindow m;
    // m.SetNetwork(netAdapter);
    // m.show();


    return a.exec();
}



