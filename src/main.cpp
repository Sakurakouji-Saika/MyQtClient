#include "app/widget.h"
#include <QtGlobal>
#include <QProcess>
#include "utils/StyleLoader.h"
#include <QApplication>
#include "utils/appconfig.h"
#include "app/mainwindow.h"
#include "Network/Service/servicemanager.h"
#include "DataBaseManage/databasemanage.h"
#include "../src/Network/Handlers/handlerregistry.h"


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

    ServiceManager *services = new ServiceManager();
    services->init();
    services->start();

    handlerregistry hr(services);
    hr.registerAll();

    Widget w; // 注入
    w.setNetwork(services);
    w.show();

    return a.exec();
}



