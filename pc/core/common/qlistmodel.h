#ifndef QSIMPLELISTMODEL_H
#define QSIMPLELISTMODEL_H

#include <QAbstractListModel>

/**
 * @brief The QListModel class implements a simple list model that stores all
 * the given roles.
 */
class QListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit QListModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    /**
     * @brief insertWithDatas inserts a new row at the given position and insert immediately the
     * given datas.
     * @param row the datas will be inserted after this row.
     * @param datas The datas are represented with a hash associating QVariants to roles
     * @param parent
     * @return
     */
    bool insertWithDatas(int row, QHash<int,QVariant> datas, const QModelIndex &parent = QModelIndex());

    
signals:
    
public slots:
    
private:
    Q_DISABLE_COPY(QListModel)
    QList<QHash<int,QVariant> > lst;
};

#endif // QSIMPLELISTMODEL_H
