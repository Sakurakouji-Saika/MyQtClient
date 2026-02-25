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

    QCoreApplication::setOrganizationName("Liu"); 
    QCoreApplication::setOrganizationDomain("Liu.dev");
    QCoreApplication::setApplicationName("1to1Chat-Portfolio");
    QCoreApplication::setApplicationVersion("0.1.0");


    AppConfig::instance().initialize();

    StyleLoader::setDebugEnabled(false);
    StyleLoader::setDebugResourceRoot(AppConfig::instance().resourcesDirectory());
    // StyleLoader::setDebugResourceRoot("C:\\Users\\Moe\\Desktop\\MyClient\\src\\resources");

    Widget w;
    w.show();

    return a.exec();
}
