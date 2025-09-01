#ifndef CHATBUBBLE_H
#define CHATBUBBLE_H

#pragma once

#include <QWidget>
#include <QTextBrowser>
#include <QFrame>
#include <QLabel>

class ChatBubbleBase : public QWidget {
    Q_OBJECT
public:
    explicit ChatBubbleBase(const QString &text, QWidget *parent = nullptr);
    virtual ~ChatBubbleBase();

    // 子类负责实现对齐，并创建 m_avatar（如果需要）
    virtual void setupAlignment() { }

    // 更新气泡宽度
    void updateBubbleWidth(int parentWidth);

protected:
    QFrame *m_bubble;
    QTextBrowser *m_textBrowser;
    QLabel *m_avatar; // 子类创建并设置样式/图片

private:
    int m_lastWidth;
    enum class Mode { Unknown, SingleLine, Wrap };
    Mode m_mode;
};

class LeftBubble : public ChatBubbleBase {
    Q_OBJECT
public:
    explicit LeftBubble(const QString &text, QWidget *parent = nullptr);
    void setupAlignment() override;
};

class RightBubble : public ChatBubbleBase {
    Q_OBJECT
public:
    explicit RightBubble(const QString &text, QWidget *parent = nullptr);
    void setupAlignment() override;
};

#endif // CHATBUBBLE_H
