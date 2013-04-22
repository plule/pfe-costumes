#ifndef COLLECTIONMANAGER_H
#define COLLECTIONMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDesktopServices>
#include <QDir>
#include <QDebug>
#include <QStringList>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlField>

#include "common/costume.h"

class CollectionManager : public QObject
{
    Q_OBJECT
public:
    explicit CollectionManager(QObject *parent = 0);
    ~CollectionManager();
    bool init(QString collectionPath);
    bool createCollectionTable();
    bool saveCostume(Costume *costume);
    Costume *loadCostume(int id);
    Costume *loadCostume(QSqlRecord record);
    QSqlTableModel* getCollectionModel();
    int getIndexOf(QString key);
    
signals:
    
public slots:
    void prepareRecord(QSqlRecord & record);

private:
    QString keySqlList(QStringList keys);
    QString keyValueList(QStringList keys);

    QSqlDatabase db;
    QSqlTableModel *collection;
    
};

#endif // COLLECTIONMANAGER_H
