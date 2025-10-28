#pragma once
#include <QString>

class AppConfig {
public:
    // 获取唯一实例
    static AppConfig& instance();

    // 初始化配置文件（可选传入 ini 路径）
    void initialize(const QString &iniFilePath = QString());

    // === Getter / Setter ===
    QString getHost() const;
    int getPort() const;
    QString getUserID() const;

    void setHost(const QString &h);
    void setPort(int p);
    void setUserID(const QString &id);

    // === 目录访问 ===
    QString dataDirectory() const;
    QString databaseDirectory() const;
    QString imagesDirectory() const;
    QString filesDirectory() const;

    // === 文件操作 ===
    bool save() const;
    bool load();

private:
    AppConfig();
    void ensureDirectories();

private:
    QString filePath;     // 配置文件路径
    QString host;         // 网络地址
    int port;             // 端口号
    QString userID;       // 用户ID

    // 目录路径
    QString dataDir;
    QString databaseDir;
    QString imagesDir;
    QString filesDir;

    bool initialized;     // 是否已初始化
};
