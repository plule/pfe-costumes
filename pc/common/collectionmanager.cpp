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
    collection = new QSqlTableModel(this, db);
    //QString path = configPath.absoluteFilePath(collection + ".db");
    db.setDatabaseName(collectionPath);
    collection->setTable("collection");
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
    collection->setTable("collection");
    return false;
}

bool CollectionManager::saveCostume(Costume *costume)
{
    if(!costume->isValid()) {
        qWarning() << "Could not save uncomplete costume";
        return false;
    } else if(costume->getId() == -1) { // New costume
        QMap<QString, QVariant> infos = costume->getInfos();
        QSqlRecord record;
        foreach(QString key, infos.keys()) {
            QSqlField field(key, costume->getInfo(key).type());
            field.setValue(costume->getInfo(key));
            record.append(field);
        }
        collection->insertRecord(-1, record);
        return collection->submit();
    } else {
        qWarning() << "Update not implemented yet.";
        return false;
    }
}

Costume *CollectionManager::loadCostume(int id)
{
    QSqlTableModel model(this, db);
    model.setTable("collection");
    model.setFilter("id='"+QString::number(id)+"'");
    model.select();
    QSqlRecord record = model.record(0);
    return loadCostume(record);
}

Costume *CollectionManager::loadCostume(QSqlRecord record)
{
    Costume *costume = new Costume();
    costume->setId(record.value("id").toInt());
    foreach(QString key, Costume::valid_informations.keys())
        costume->setInfo(key, record.value(key));
    return costume;
}

QSqlTableModel *CollectionManager::getCollectionModel()
{
    return collection;
}

QString CollectionManager::keySqlList(QStringList keys)
{
    return keys.join(", ");
}

QString CollectionManager::keyValueList(QStringList keys)
{
    return ":" + keys.join(", :");
}
