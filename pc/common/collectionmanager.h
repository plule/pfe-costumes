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

#include "common/costume.h"

class CollectionManager : public QObject
{
    Q_OBJECT
public:
    explicit CollectionManager(QObject *parent = 0);
    ~CollectionManager();
    bool init(QString collectionPath);
    bool createCollectionTable();
    bool saveCostume(Costume costume);
    
signals:
    
public slots:

private:
    QString keySqlList();
    QString keyValueList();
    QStringList keyList();

    QSqlDatabase db;
    
};

#endif // COLLECTIONMANAGER_H
