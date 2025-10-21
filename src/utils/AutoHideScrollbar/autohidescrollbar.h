#pragma once

#include <QObject>
#include <QAbstractScrollArea>
#include <QTimer>
#include <QEvent>
#include <QScrollBar>

/*
 * AutoHideScrollbar
 *  - 支持 QAbstractScrollArea 及其子类（QListView/QListWidget/QTreeView/...）
 *  - timeoutMs: 超时时间（毫秒），默认 10000（10s）
 *  - parent: 可选 QObject parent（通常把控件本身作为 parent）
 */
class AutoHideScrollbar : public QObject {
    Q_OBJECT
public:
    explicit AutoHideScrollbar(QAbstractScrollArea* area,
                               int timeoutMs = 10000,
                               QObject* parent = nullptr);
    ~AutoHideScrollbar() override = default;

    // 改变超时时间（毫秒）
    void setTimeout(int ms);

    // 立即显示并重启计时器
    void showAndRestart();

    // 停用（停止计时器并确保滚动条显示或隐藏）
    void stop(bool showImmediate = false);

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;

private:
    QAbstractScrollArea* m_area = nullptr;
    QTimer* m_timer = nullptr;
    int m_timeout = 10000;

    void ensureSetup();
};
