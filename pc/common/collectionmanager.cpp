#include "collectionmanager.h"

CollectionManager::CollectionManager(QObject *parent) :
    QObject(parent)
{
}

CollectionManager::~CollectionManager()
{
    db.close();
}

bool CollectionManager::init(QString collectionPath)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    //QString path = configPath.absoluteFilePath(collection + ".db");
    db.setDatabaseName(collectionPath);

    return db.open();
}

bool CollectionManager::createCollectionTable()
{
    QString query = "create table collection (id integer primary key";
    QSqlQuery sqlquery;
    foreach(QString key, Costume::valid_informations.keys()) {
        Costume_info info = Costume::valid_informations.value(key);

        if(Costume::sql_types.keys().contains(info.type)) {
            QString type = Costume::sql_types.value(info.type);
            query.append(", " + key + " " + type);
        } else {
            qWarning() << "Field \"" + key + "\" is not supported for storage in the database.";
        }
    }

    query.append(")");
    qDebug() << query;
    bool ret = sqlquery.exec(query);
    if(!ret)
        qDebug() << sqlquery.lastError();
    return false;
}

bool CollectionManager::saveCostume(Costume costume)
{
    if(!costume.isValid()) {
        qWarning() << "Could not save uncomplete costume";
        return false;
    } else if(costume.getId() == -1) { // New costume
        QSqlQuery query;
        query.prepare("INSERT INTO collection ("+keySqlList()+")"
                      "VALUES ("+keyValueList()+")");
        foreach(QString key, Costume::valid_informations.keys()) {
            query.bindValue(":"+key, costume.getInfo(key));
        }
        if(query.exec())
            return true;
        else {
            qWarning() << query.lastError();
            return false;
        }
    } else { // Update existing costume
        qWarning() << "Updating costume not supported yet.";
        return false;
    }
}

QString CollectionManager::keySqlList()
{
    return keyList().join(", ");
}

QString CollectionManager::keyValueList()
{
    return ":" + keyList().join(", :");
}

QStringList CollectionManager::keyList()
{
    return Costume::valid_informations.keys();
}
