#include "app/widget.h"
#include <QtGlobal>
#include <QProcess>
#include "utils/StyleLoader.h"
#include <QApplication>
#include "utils/appconfig.h"
#include "app/mainwindow.h"

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



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    StyleLoader::setDebugEnabled(true);
    StyleLoader::setDebugResourceRoot("C:/Users/Moe/Desktop/MyClient/src/resources");


    my_version();



    // 一次性加载配置，之后只读
    AppConfig::initialize("://config/app.ini");


    Widget w;
    w.show();

    MainWindow m;
    m.show();



    Registration_Page   m_pg;
    m_pg.show();

    return a.exec();
}



