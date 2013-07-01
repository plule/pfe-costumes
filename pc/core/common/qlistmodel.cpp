#include "qlistmodel.h"

QListModel::QListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int QListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return lst.count();
}

QVariant QListModel::data(const QModelIndex &index, int role) const
{
    return lst.value(index.row()).value(role,QVariant());
}

bool QListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.row() >= 0 && index.row() < lst.size()) {
        QHash<int,QVariant> oldValue = lst.value(index.row());
        oldValue.insert(role, value);
        lst.replace(index.row(), oldValue);
        emit dataChanged(index,index,QVector<int>() << role);
        return true;
    }
    return false;
}

bool QListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (count < 1 || row < 0 || row > rowCount(parent))
        return false;
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int r = 0; r < count; ++r)
        lst.insert(row, QHash<int,QVariant>());
    endInsertRows();
    return true;
}

bool QListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0 || row < 0 || (row + count) > rowCount(parent))
        return false;
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int r = 0; r < count; ++r)
        lst.removeAt(row);
    endRemoveRows();
    return true;
}
