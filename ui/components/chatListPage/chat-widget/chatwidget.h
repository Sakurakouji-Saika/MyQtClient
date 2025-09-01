#ifndef CHATWIDGET_H
#define CHATWIDGET_H


#pragma once

#include <QWidget>
#include <QTimer>
#include "chatbubble.h"

class QScrollArea;
class QVBoxLayout;
class QLineEdit;
class QScrollBar;
class ChatBubbleBase;

class ChatWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChatWidget(QWidget *parent = nullptr);
    void addMessage(const QString &text, bool isSelf);

protected:
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void updateAllBubbleWidths();

private:
    QScrollArea *m_scroll;
    QWidget *m_messagesWidget;
    QVBoxLayout *m_messagesLayout;
    QLineEdit *m_input;
    int m_lastViewportWidth;
    QTimer *m_updateTimer;
};


#endif // CHATWIDGET_H
