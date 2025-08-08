#ifndef IPLINEEDIT_H
#define IPLINEEDIT_H

#include <QLineEdit>
#include <QGroupBox>
#include <QLabel>

#define IP_INPUT_SIZE 4
class IPLineEdit : public QGroupBox
{
public:
    IPLineEdit(QWidget *parent=0);
    ~IPLineEdit();

    QString text();

private:
    QLineEdit *m_lineEdit[IP_INPUT_SIZE];
    QLabel *labelDot[3];

private:
    int getIndex(QLineEdit *pLineEdit);

public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};




#endif // IPLINEEDIT_H
