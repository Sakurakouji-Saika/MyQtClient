#pragma once
#include <QString>


class AppConfig{
public:
    // 只应在 main() 开头调用一次
    static void initialize(const QString & iniFilePath);

    // 只读访问接口
    static const QString& host();
    static int port();

private:
    AppConfig() = delete;
    ~AppConfig() = delete;

    static bool initialized_;
    static QString m_host;
    static int m_port;
};
