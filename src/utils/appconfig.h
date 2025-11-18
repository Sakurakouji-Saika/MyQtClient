#pragma once

#ifndef APPCONFIG_H
#define APPCONFIG_H


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
    int getUserID() const;
    QString getFileHost() const;
    int getFilePort() const;

    void setHost(const QString &h);
    void setPort(int p);
    void setUserID(const int &id);
    void setFileHost(const QString &host);
    void setFilePort(int p);

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
    QString filePath;       // 配置文件路径
    QString host;           // 网络地址
    int port;               // 端口号
    int userID;         // 用户ID

    QString FileHost;       // 文件服务器地址
    int FilePort;           // 文件服务器端口

    // 目录路径
    QString dataDir;        // 数据目录
    QString databaseDir;    // 存放用户数据的目录
    QString imagesDir;      // 存放用户图片的目录
    QString filesDir;       // 存放用户文件的目录

    bool initialized;     // 是否已初始化
};

#endif // APPCONFIG_H
