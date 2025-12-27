#ifndef STYLELOADER_H
#define STYLELOADER_H

#include <QObject>
#include <QWidget>
#include <QFile>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QApplication>
#include <QDebug>
#include <QHash>
#include <QPointer>

class StyleLoader : public QObject
{
    Q_OBJECT
public:
    // 设置调试资源根目录（用于热重载）
    static void setDebugResourceRoot(const QString& path);

    // 设置调试输出开关
    static void setDebugEnabled(bool enabled);

    // 为应用程序全局加载样式
    static void loadGlobalStyle(const QString& qrcPath);

    // 为特定窗口加载样式
    static void loadWidgetStyle(QWidget* widget, const QString& qrcPath);

private:
    static QString s_debugResourceRoot;
    static bool s_debugEnabled;
    static QHash<QString, QPointer<QFileSystemWatcher>> s_watcherMap;

    static void applyStyle(QObject* target, const QString& qrcPath);
    static bool loadFromResource(const QString& qrcPath, QString& out);
    static bool loadFromFileSystem(const QString& path, QString& out);
    static void setStyleSheet(QObject* target, const QString& styleSheet);
    static QString convertQrcPathToFsPath(const QString& qrcPath);
    static void setupFileWatcher(QObject* target, const QString& path);
    static void reloadStyle(QObject* target, const QString& path);

    // 调试输出方法
    static void debugLog(const QString& message);
    static void warningLog(const QString& message);
    static void errorLog(const QString& message);
};

#endif // STYLELOADER_H
