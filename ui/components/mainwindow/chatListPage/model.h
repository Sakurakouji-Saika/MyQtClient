#ifndef MODEL_H
#define MODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QList>
#include <QSize>
#include "recent_data.h"
#include "../../Src/DataBaseManage/model/ChatRecord.h"

class Model : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = nullptr);

    // QAbstractItemModel interface

public:
    void addItem(const Recent_Data &item);
    bool removeRow(int row);
    Recent_Data get_Row(int row);
    void my_diy_sort(bool descending);
    // void update_Item_At(int row, const Recent_Data &newData);
    void addItemFront(const Recent_Data &data);


public:
    virtual int rowCount(const QModelIndex &parent  = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

private:
    QList<Recent_Data> m_list_data;

};

#endif // MODEL_H
