#include "AppConfig.h"
#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

// ===== 单例实例 =====
AppConfig& AppConfig::instance() {
    static AppConfig inst;
    return inst;
}

// ===== 构造函数 =====
AppConfig::AppConfig()
    : host("192.168.3.241"),
    port(60100),
    userID(3),
    FileHost("192.168.3.241"),
    FilePort(60101),
    initialized(false)
{
}

// ===== 初始化 =====
void AppConfig::initialize(const QString &iniFilePath)
{
    if (initialized) {
        qDebug() << "AppConfig 已初始化，跳过";
        return;
    }

    // 程序目录
    QString appDir  = QCoreApplication::applicationDirPath();

    dataDir         = QDir(appDir).filePath("data");
    databaseDir     = QDir(dataDir).filePath("database");
    filesDir        = QDir(dataDir).filePath(QStringLiteral("files"));
    imagesDir       = QDir(dataDir).filePath(QStringLiteral("images"));


    ensureDirectories();

    // 配置文件路径
    if (!iniFilePath.isEmpty()) {
        filePath = iniFilePath;
    } else {
        filePath = QDir(dataDir).filePath("config.ini");
    }

    QFileInfo fi(filePath);
    if (fi.exists() && fi.isFile()) {
        load();
    } else {
        qWarning() << "配置文件不存在，将使用默认值";
        save();
    }

    initialized = true;
    qDebug() << "AppConfig 初始化完成: host=" << host << ", port=" << port;
}

// ===== Getter =====
QString AppConfig::getHost() const { return host; }
int AppConfig::getPort() const { return port; }
int AppConfig::getUserID() const { return userID; }

QString AppConfig::getFileHost() const{ return FileHost; }

int AppConfig::getFilePort() const
{
    return FilePort;
}


// ===== Setter =====
void AppConfig::setHost(const QString &h) { host = h; save(); }
void AppConfig::setPort(int p) { port = p; save(); }
void AppConfig::setUserID(const int &id) { userID = id; save(); }

void AppConfig::setFileHost(const QString &host){ FileHost = host; save(); }

void AppConfig::setFilePort(int p)
{
    FilePort = p;
}


// ===== 目录访问 =====
QString AppConfig::dataDirectory() const { return dataDir; }
QString AppConfig::databaseDirectory() const { return databaseDir; }
QString AppConfig::imagesDirectory() const { return imagesDir; }
QString AppConfig::filesDirectory() const { return filesDir; }

// ===== 保存配置 =====
bool AppConfig::save() const
{
    if (filePath.isEmpty()) return false;

    QDir().mkpath(QFileInfo(filePath).absolutePath());

    QSettings s(filePath, QSettings::IniFormat);
    s.setValue("network/host", host);
    s.setValue("network/port", port);
    s.setValue("network/fileHost", host);
    s.setValue("network/filePort", port);
    s.setValue("user/user_id", userID);
    s.sync();

    if (s.status() != QSettings::NoError) {
        qWarning() << "AppConfig::save(): 写入配置时发生错误";
        return false;
    }
    return true;
}

// ===== 读取配置 =====
bool AppConfig::load()
{
    if (filePath.isEmpty()) return false;
    QFileInfo fi(filePath);
    if (!fi.exists()) return false;

    QSettings s(filePath, QSettings::IniFormat);
    host = s.value("network/host", host).toString();
    port = s.value("network/port", port).toInt();
    FileHost = s.value("network/fileHost",FileHost).toString();
    FilePort = s.value("network/filePort",FilePort).toInt();
    userID = s.value("user/user_id", userID).toInt();

    return true;
}

// ===== 创建目录 =====
void AppConfig::ensureDirectories()
{
    QDir dir;
    auto ensure = [&](const QString &path) {
        if (!dir.exists(path)) {
            if (!dir.mkpath(path)) {
                qWarning() << "无法创建目录:" << path;
            }
        }
    };

    ensure(dataDir);
    ensure(databaseDir);
    ensure(imagesDir);
    ensure(filesDir);
}
