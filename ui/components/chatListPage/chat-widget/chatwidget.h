#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPointer>
#include "chatmodel.h"
#include "chatdelegate.h"

class QListView;
class QLineEdit;

class ChatWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChatWidget(QWidget *parent = nullptr);
    void addMessage(bool isSelf,QString avatar_url,const QString &text);
    void clearAllMsg();

protected:
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void updateDelegateWidth();
    void scrollToBottom();

private:
    QListView *m_listView;
    QPointer<ChatModel> m_model; // 使用 QPointer 防止悬垂指针
    ChatDelegate *m_delegate;
    QLineEdit *m_input;
    int m_lastViewportWidth;
    QTimer *m_updateTimer;
};

#endif // CHATWIDGET_H
