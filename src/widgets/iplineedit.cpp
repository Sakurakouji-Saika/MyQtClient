#include "iplineedit.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QIntValidator>
#include <QEvent>
#include <QKeyEvent>
#include <QString>

#include <QApplication>
#include <QClipboard>


IPLineEdit::IPLineEdit(QWidget *parent)
    :QGroupBox(parent)
{

    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
    hboxLayout->setContentsMargins(10, 0, 10, 0);


    for(int i=0;i<IP_INPUT_SIZE;i++){
        m_lineEdit[i] = new QLineEdit(this);

        //保存线条编辑搜分使用边框绘制本身【去掉边框】
        m_lineEdit[i]->setFrame(false);

        // 文本居中
        m_lineEdit[i]->setAlignment(Qt::AlignCenter);

        m_lineEdit[i]->setMaximumWidth(65);
        // 输入框最大位数
        m_lineEdit[i]->setMaxLength(3);

        // 消除布局中的空隙，居中
        m_lineEdit[i]->setAlignment(Qt::AlignCenter);

        // 保存小部件的默认布局
        m_lineEdit[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        // 完整匹配 0–255
        QRegularExpression re("^(25[0-5]|2[0-4]\\d|[01]?\\d?\\d)$");
        auto *validator = new QRegularExpressionValidator(re, m_lineEdit[i]);
        m_lineEdit[i]->setValidator(validator);


        m_lineEdit[i]->installEventFilter(this);

        hboxLayout->addWidget(m_lineEdit[i]);

        if(i<IP_INPUT_SIZE-1){
            labelDot[i] = new QLabel(this);
            labelDot[i]->setText(".");
            labelDot[i]->setFixedWidth(2);
            hboxLayout->addWidget(labelDot[i]);
        }

    }
}

IPLineEdit::~IPLineEdit()
{
    for (int i = 0; i < IP_INPUT_SIZE; ++i) {
        delete m_lineEdit[i];
        m_lineEdit[i] = nullptr;
    }

    for(int i=0;i<3;i++){
        delete labelDot[i];
        labelDot[i] = nullptr;
    }
}

QString IPLineEdit::text()
{
    QString ip;
    for(int i=0;i<IP_INPUT_SIZE;i++){
        ip.append(m_lineEdit[i]->text());
        if(3!=i){
            ip.append(".");
        }
    }

    return ip;
}

void IPLineEdit::setIP(QString ip)
{
    QStringList data = ip.split(".");
    for(int i =0; i < IP_INPUT_SIZE; i++){
        m_lineEdit[i]->setText(data[i]);
    }
}

int IPLineEdit::getIndex(QLineEdit *pLineEdit)
{
    int index = -1;
    for(int i=0;i<IP_INPUT_SIZE;i++){
        if(pLineEdit == m_lineEdit[i]){
            index = i;
        }
    }
    return index;
}

bool IPLineEdit::eventFilter(QObject *obj, QEvent *event)
{
    if(children().contains(obj) && QEvent::KeyPress == event->type()){
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);

        // 4个 输入框里面正在输入的那个
        QLineEdit *pCurrent = qobject_cast<QLineEdit*>(obj);

        // —— 1. 先拦截粘贴（Ctrl+V、Shift+Insert 等） ——
        if (event->type() == QEvent::KeyPress)
        {
            auto *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->matches(QKeySequence::Paste))
            {
                // 读取剪贴板
                QString text = QGuiApplication::clipboard()->text().trimmed();
                // 格式校验： \d{1,3}(.\d{1,3}){0,3}
                static const QRegularExpression rx(R"(^\d{1,3}(\.\d{1,3}){0,3}$)");
                if (rx.match(text).hasMatch())
                {
                    // 拆分并填充
                    const QStringList parts = text.split('.');
                    for (int i = 0; i < parts.size() && i < IP_INPUT_SIZE; ++i)
                    {
                        int v = parts[i].toInt();
                        v = qBound(0, v, 255);
                        m_lineEdit[i]->setText(QString::number(v));
                    }
                    // 焦点移到最后一段并全选
                    int last = qMin(parts.count(), IP_INPUT_SIZE) - 1;
                    m_lineEdit[last]->setFocus();
                    m_lineEdit[last]->selectAll();
                }
                // 无论合法与否都吞掉这次粘贴
                return true;
            }
        }

        switch(keyEvent->key()){
            case Qt::Key_0:
            case Qt::Key_1:
            case Qt::Key_2:
            case Qt::Key_3:
            case Qt::Key_4:
            case Qt::Key_5:
            case Qt::Key_6:
            case Qt::Key_7:
            case Qt::Key_8:
            case Qt::Key_9:
            {
                int idx = getIndex(pCurrent);
                int len = pCurrent->text().length();
                int max = pCurrent->maxLength(); // 3

                if (len+1 >= max) {
                    // 当前已经满了，跳到下一个格子
                    if (idx >= 0 && idx < IP_INPUT_SIZE - 1) {
                        m_lineEdit[idx + 1]->setFocus();
                        // 不要在当前格子消费这次按键，返回 false 让新的焦点接收
                        return false;
                    }
                }
                return QObject::eventFilter(obj, event);
            }
            break;
            case Qt::Key_Left:
            {
                int pos = pCurrent->cursorPosition();
                if (pos == 0) {
                    int idx = getIndex(pCurrent);
                    if (idx > 0) {
                        QLineEdit *prev = m_lineEdit[idx - 1];
                        prev->setFocus();
                        prev->setCursorPosition(prev->text().length());
                        return true;
                    }
                }
                return QObject::eventFilter(obj, event);
            }

            case Qt::Key_Right:
            {
                int pos = pCurrent->cursorPosition();
                int len = pCurrent->text().length();
                if (pos == len) {
                    int idx = getIndex(pCurrent);
                    if (idx >= 0 && idx < IP_INPUT_SIZE - 1) {
                        QLineEdit *next = m_lineEdit[idx + 1];
                        next->setFocus();
                        next->setCursorPosition(0);
                        return true;
                    }
                }
                return QObject::eventFilter(obj, event);
            }
            break;
            case Qt::Key_Backspace:
            {
                // 光标在当前框最左侧
                if (pCurrent->cursorPosition() == 0) {
                    int idx = getIndex(pCurrent);
                    if (idx > 0) {
                        QLineEdit *prev = m_lineEdit[idx - 1];
                        // 跳到前一个框
                        prev->setFocus();
                        // 把它的最后一个字符删掉
                        QString txt = prev->text();
                        if (!txt.isEmpty()) {
                            txt.chop(1);
                            prev->setText(txt);
                        }
                        // 光标放到新的文本末尾
                        prev->setCursorPosition(prev->text().length());
                    }
                    // 吞掉当前事件（因为我们在前一个框已经处理了退格）
                    return true;
                }
                // 否则让默认行为（删除当前框中光标左边的字符）
                return QObject::eventFilter(obj, event);
            }
            break;
            case Qt::Key_Period:   // 英文状态下的 '.'
            // case Qt::Key_Decimal:  // 小键盘上的 '.'
            {
                int idx = getIndex(pCurrent);
                if (idx >= 0 && idx < IP_INPUT_SIZE - 1) {
                    // 跳到下一个框并全选
                    QLineEdit *next = m_lineEdit[idx + 1];
                    next->setFocus();
                    next->selectAll();
                }
                // 吞掉这个事件，不让 '.' 出现在任何框里
                return QObject::eventFilter(obj, event);
            }
            break;
        }

            if (keyEvent->text() == ".") {
                int idx = getIndex(pCurrent);
                if (idx >= 0 && idx < IP_INPUT_SIZE - 1) {
                    QLineEdit *next = m_lineEdit[idx + 1];
                    next->setFocus();
                    next->selectAll();
                }
                return QObject::eventFilter(obj, event);
            }
    }

    return QObject::eventFilter(obj, event);

}
