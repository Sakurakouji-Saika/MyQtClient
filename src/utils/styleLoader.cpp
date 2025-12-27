#include "styleLoader.h"

QString StyleLoader::s_debugResourceRoot = "";
bool StyleLoader::s_debugEnabled = false; // 默认关闭调试输出
QHash<QString, QPointer<QFileSystemWatcher>> StyleLoader::s_watcherMap;

void StyleLoader::setDebugResourceRoot(const QString& path)
{
    s_debugResourceRoot = path;
    debugLog("设置调试资源根目录: " + path);
}

void StyleLoader::setDebugEnabled(bool enabled)
{
    s_debugEnabled = enabled;
    debugLog(QString("调试输出已%1").arg(enabled ? "启用" : "禁用"));
}

void StyleLoader::loadGlobalStyle(const QString& qrcPath)
{
    debugLog("加载全局样式: " + qrcPath);
    applyStyle(nullptr, qrcPath);
}

void StyleLoader::loadWidgetStyle(QWidget* widget, const QString& qrcPath)
{
    if (!widget) {
        warningLog("尝试为nullptr窗口加载样式: " + qrcPath);
        return;
    }

    debugLog(QString("为窗口加载样式 [%1: %2]: %3")
                 .arg(widget->objectName())
                 .arg(widget->metaObject()->className())
                 .arg(qrcPath));

    applyStyle(widget, qrcPath);
}

void StyleLoader::applyStyle(QObject* target, const QString& qrcPath)
{
    QString styleSheetContent;
    QString targetName = target ?
                             QString("%1 [%2]").arg(target->objectName()).arg(target->metaObject()->className()) :
                             "全局应用";

    debugLog(QString("开始应用样式到: %1 | 路径: %2").arg(targetName).arg(qrcPath));

#ifdef QT_DEBUG
    // 在调试模式下，尝试从文件系统加载
    if (!s_debugResourceRoot.isEmpty()) {
        QString fsPath = convertQrcPathToFsPath(qrcPath);
        debugLog("尝试文件系统路径: " + fsPath);

        if (loadFromFileSystem(fsPath, styleSheetContent)) {
            setStyleSheet(target, styleSheetContent);
            debugLog("成功从文件系统加载样式表");
            setupFileWatcher(target, fsPath);
            return;
        } else {
            warningLog("文件系统加载失败: " + fsPath);
        }
    }
#endif

    // 常规方式从qrc资源加载
    if (loadFromResource(qrcPath, styleSheetContent)) {
        setStyleSheet(target, styleSheetContent);
        debugLog("成功从资源文件加载样式表");
    } else {
        errorLog("样式表加载失败: " + qrcPath);
    }
}

bool StyleLoader::loadFromResource(const QString& qrcPath, QString& out)
{
    debugLog("尝试加载资源: " + qrcPath);

    // 检查资源是否注册
    if (!QFile::exists(qrcPath)) {
        errorLog("资源路径不存在: " + qrcPath);
        return false;
    }

    QFile file(qrcPath);
    if (!file.open(QIODevice::ReadOnly)) {
        errorLog(QString("资源打开错误: %1 | 路径: %2")
                     .arg(file.errorString())
                     .arg(qrcPath));
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    if (data.isEmpty()) {
        warningLog("资源内容为空: " + qrcPath);
        return false;
    }

    out = QString::fromUtf8(data);
    debugLog(QString("成功加载资源: 大小=%1 字节 | 路径=%2")
                 .arg(data.size())
                 .arg(qrcPath));

    return true;
}

bool StyleLoader::loadFromFileSystem(const QString& path, QString& out)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        warningLog(QString("文件打开失败: %1 | 路径: %2")
                       .arg(file.errorString())
                       .arg(path));
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    if (data.isEmpty()) {
        warningLog("文件内容为空: " + path);
        return false;
    }

    out = QString::fromUtf8(data);
    debugLog(QString("成功加载文件: 大小=%1 字节 | 路径=%2")
                 .arg(data.size())
                 .arg(path));

    return true;
}

void StyleLoader::setStyleSheet(QObject* target, const QString& styleSheet)
{
    if (target) {
        if (QWidget* widget = qobject_cast<QWidget*>(target)) {
            QString objInfo = QString("%1 [%2]")
            .arg(widget->objectName())
                .arg(widget->metaObject()->className());

            debugLog(QString("应用样式到: %1").arg(objInfo));

            // 只在调试模式下记录样式变化
            if (s_debugEnabled) {
                QString oldStyle = widget->styleSheet();
                widget->setStyleSheet(styleSheet);
                QString newStyle = widget->styleSheet();

                debugLog(QString("  旧样式: 长度=%1").arg(oldStyle.length()));
                debugLog(QString("  新样式: 长度=%1").arg(newStyle.length()));

                if (newStyle == oldStyle) {
                    warningLog("样式未改变! 可能被忽略");
                }

                // 打印样式预览
                if (!newStyle.isEmpty()) {
                    debugLog("样式预览:");
                    QStringList lines = newStyle.split('\n');
                    for (int i = 0; i < qMin(5, lines.size()); i++) {
                        debugLog("  " + lines[i]);
                    }
                    if (lines.size() > 5) {
                        debugLog(QString("  ... 还有 %1 行").arg(lines.size() - 5));
                    }
                }
            } else {
                // 非调试模式直接设置
                widget->setStyleSheet(styleSheet);
            }
        } else {
            warningLog("目标对象不是QWidget: " + QString(target->metaObject()->className()));
        }
    } else {
        qApp->setStyleSheet(styleSheet);
        debugLog(QString("全局样式已设置: 长度=%1").arg(styleSheet.length()));
    }
}

QString StyleLoader::convertQrcPathToFsPath(const QString& qrcPath)
{
    if (qrcPath.startsWith(":/")) {
        QString path = s_debugResourceRoot + qrcPath.mid(1);
        debugLog(QString("转换qrc路径: %1 → %2").arg(qrcPath).arg(path));
        return path;
    }
    debugLog("使用原始路径: " + qrcPath);
    return qrcPath; // 已经是文件系统路径
}

void StyleLoader::setupFileWatcher(QObject* target, const QString& path)
{
#ifdef QT_DEBUG
    debugLog("设置文件监控: " + path);

    // 清理已失效的监视器
    auto it = s_watcherMap.begin();
    while (it != s_watcherMap.end()) {
        if (it.value().isNull()) {
            it = s_watcherMap.erase(it);
        } else {
            ++it;
        }
    }

    // 复用已有监视器
    QFileSystemWatcher* watcher = nullptr;
    if (s_watcherMap.contains(path) && !s_watcherMap[path].isNull()) {
        watcher = s_watcherMap[path];
        debugLog("复用现有文件监视器");
    } else {
        watcher = new QFileSystemWatcher();
        s_watcherMap[path] = watcher;
        debugLog("创建新文件监视器");
    }

    // 确保路径被监控
    if (!watcher->files().contains(path)) {
        if (!watcher->addPath(path)) {
            warningLog("添加文件监控失败: " + path);
            return;
        }
    }

    // 使用QPointer保护目标
    QPointer<QObject> guardedTarget(target);

    // 连接文件变化信号
    QObject::connect(watcher, &QFileSystemWatcher::fileChanged,
                     [guardedTarget, path, watcher](const QString& changedPath) {
                         debugLog(QString("检测到文件变更: %1").arg(changedPath));

                         // 重新添加监控（文件可能被编辑器临时删除）
                         QTimer::singleShot(300, [guardedTarget, path, watcher]() {
                             if (guardedTarget.isNull()) {
                                 debugLog("目标对象已销毁，跳过重载");
                                 return;
                             }

                             debugLog("尝试重新添加文件监控: " + path);

                             if (QFile::exists(path)) {
                                 watcher->addPath(path);
                                 reloadStyle(guardedTarget.data(), path);
                             } else {
                                 warningLog("文件不存在，无法重新监控: " + path);
                             }
                         });
                     });

    // 自动清理监视器
    if (target) {
        QObject::connect(target, &QObject::destroyed, [path]() {
            if (s_watcherMap.contains(path) && !s_watcherMap[path].isNull()) {
                s_watcherMap[path]->deleteLater();
                s_watcherMap.remove(path);
                debugLog("目标销毁，移除文件监视器: " + path);
            }
        });
    }
#endif
}

void StyleLoader::reloadStyle(QObject* target, const QString& path)
{
    debugLog("尝试热重载样式: " + path);

    QString content;
    if (loadFromFileSystem(path, content)) {
        setStyleSheet(target, content);
        debugLog("热重载成功完成");
    } else {
        warningLog("热重载失败: " + path);
    }
}

// =============== 日志方法 ===============

void StyleLoader::debugLog(const QString& message)
{
    if (s_debugEnabled) {
        qDebug().noquote() << "[StyleLoader DEBUG]" << message;
    }
}

void StyleLoader::warningLog(const QString& message)
{
    if (s_debugEnabled) {
        qWarning().noquote() << "[StyleLoader WARNING]" << message;
    }
}

void StyleLoader::errorLog(const QString& message)
{
    // 错误日志总是输出
    qCritical().noquote() << "[StyleLoader ERROR]" << message;
}
