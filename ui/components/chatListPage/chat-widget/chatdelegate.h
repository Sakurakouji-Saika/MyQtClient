#ifndef CHATDELEGATE_H
#define CHATDELEGATE_H

#include <QStyledItemDelegate>
#include <QTextBrowser>
#include <QFrame>
#include <QLabel>
#include "chatmodel.h"

class ChatDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit ChatDelegate(QObject *parent = nullptr);

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void updateWidth(int width);

private:
    int m_availableWidth;
    mutable QHash<QString, QSize> m_sizeCache;
};

#endif // CHATDELEGATE_H
