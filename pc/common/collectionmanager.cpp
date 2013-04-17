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
    /*foreach(Costume_info info, Costume::default_infos) {
        QString type = "";
        if(info.type == ShortString)
            type = "varchar(256)";
        else if(info.type == LongString)
            type = "varchar(4096)";
        else if(info.type == Number)
            type = "integer";
        else if(info.type == Files)
            qWarning() << "File storage unimplemented yet";
        else
            qWarning()<< "Warning unknown sql storage type for " + info.key;

        if(type != "") {
            query.append(", " + info.key + " " + type);
        }
    }
    query.append(")");
    qDebug() << query;
    bool ret = sqlquery.exec(query);
    if(!ret)
        qDebug() << sqlquery.lastError();
    return ret;*/
}

bool CollectionManager::addCostume(Costume costume)
{
    QSqlQuery query;
    query.prepare("INSERT INTO collection ("+keySqlList()+")"
                  "VALUES ("+keyValueList()+")");
    QStringList validKeys = keyList();
//    foreach(Costume_info info, costume.getInformations()) {
//        if(validKeys.contains(info.key)) {
//            query.bindValue(":"+info.key, info.value);
//        }
//    }
//    return query.exec();
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
    QStringList ret;
//    foreach(Costume_info info, Costume::default_infos)
//        if(info.type == ShortString || info.type == LongString || info.type == Number)
//            ret.append(info.key);
//    return ret;
}
