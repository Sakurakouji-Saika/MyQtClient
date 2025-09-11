#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QTimer>
#include "chatmodel.h"
#include "chatdelegate.h"

class QListView;
class QLineEdit;

class ChatWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChatWidget(QWidget *parent = nullptr);
    void addMessage(bool isSelf,QString avatar_url,const QString &text);

protected:
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void updateDelegateWidth();
    void scrollToBottom();

private:
    QListView *m_listView;
    ChatModel *m_model;
    ChatDelegate *m_delegate;
    QLineEdit *m_input;
    int m_lastViewportWidth;
    QTimer *m_updateTimer;
};

#endif // CHATWIDGET_H
