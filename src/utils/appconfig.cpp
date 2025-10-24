#include "AppConfig.h"
#include <QSettings>
#include <QFileInfo>
#include <QDebug>

bool    AppConfig::initialized_ = false;
QString AppConfig::m_host       = QStringLiteral("127.0.0.1");
int     AppConfig::m_port       = 60100;
QString AppConfig::m_user_id = "user_1000";
QString AppConfig::FilePath = "";


void AppConfig::initialize(const QString &iniFilePath)
{

    FilePath =iniFilePath;

    if(initialized_){
        qDebug() << "AppConfig::initialize(): 已初始化，跳过重复调用";
        return;
    }
    initialized_ = true;
    QFileInfo fi(iniFilePath);
    if (!fi.exists()) {
        qWarning() << "AppConfig::initialize(): 配置文件不存在:" << iniFilePath
                   << "，将使用默认值";
        return;
    }

    QSettings settings(iniFilePath,QSettings::IniFormat);
    if(settings.status() != QSettings::NoError){
        qWarning() << "AppConfig::initialize(): 读取配置时发生错误";
    }

    settings.beginGroup("network");
    m_host = settings.value("host",m_host).toString();
    m_port = settings.value("port", m_port).toInt();
    settings.endGroup();

    settings.beginGroup("User");
    m_user_id = settings.value("m_user_id",m_user_id).toString();
    settings.endGroup();


    qDebug() << "AppConfig::initialize(): 已加载 network.host =" << m_host
             << ", port =" << m_port;
}

const QString &AppConfig::host()
{
    return m_host;
}

int AppConfig::port()
{
    return m_port;
}

void AppConfig::setUsetID(QString _id)
{
    m_user_id = _id;

    QSettings settings(FilePath,QSettings::IniFormat);
    settings.beginGroup("User");
    m_user_id = settings.value("m_user_id",m_host).toString();
    settings.endGroup();
}

QString AppConfig::UserID()
{
    return m_user_id;
}
