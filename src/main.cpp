#include "app/widget.h"
#include <QtGlobal>
#include <QProcess>
#include "utils/styleLoader.h"
#include <QApplication>
#include "utils/appconfig.h"
#include "app/mainwindow.h"
#include "Network/Service/servicemanager.h"
#include "DataBaseManage/databasemanage.h"
#include "../src/Network/Handlers/handlerregistry.h"



class Widget;


int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    QApplication a(argc, argv);

    AppConfig::instance().initialize();

    StyleLoader::setDebugEnabled(true);
    StyleLoader::setDebugResourceRoot(AppConfig::instance().resourcesDirectory());
    // StyleLoader::setDebugResourceRoot("C:\\Users\\Moe\\Desktop\\MyClient\\src\\resources");



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



