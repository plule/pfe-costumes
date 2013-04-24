#include "collection.h"

QMap<QString, Costume_info> Collection::valid_informations;
QMap<Costume_info_type, QString> Collection::sql_types;
int Costume_info::last_order;

Collection::Collection(QObject *parent) :
    QObject(parent)
{
}

Collection::Collection(QObject *parent, QString collectionPath) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(collectionPath);
    model = new QSqlTableModel(this, db);
    lastId = 0;

    valid = db.open();
    if(valid) {
        if(!db.tables().contains("collection"))
            createCollectionTable(); //todo test collection table
        QSqlQuery q(db);
        q.exec("SELECT MAX(id) FROM collection");
        if(q.next()) {
            lastId = q.value(0).toInt();
        }
    }
    model->setTable("collection");
    model->setFilter("notdeleted == 1");
    model->select();
    collectionDir = QFileInfo(collectionPath).absoluteDir();
}

Collection::~Collection()
{
    db.close();
}

bool Collection::createCollectionTable()
{
    QString query = "create table collection (";
    QSqlQuery sqlquery;
    QList<QPair<Costume_info, QString> > orderedInfos = sortedValidInformations();
    for(int i = 0; i < orderedInfos.length(); i++) {
        QPair<Costume_info, QString> pair = orderedInfos.at(i);
        QString key = pair.second;
        Costume_info info = pair.first;

        if(sql_types.keys().contains(info.type)) {
            QString type = sql_types.value(info.type);
            query.append(key + " " + type);
            if(i != orderedInfos.length()-1)
                query.append(", ");
        } else {
            qWarning() << "Field \"" + key + "\" is not supported for storage in the database.";
        }
    }
    query.append(")");
    bool ret = sqlquery.exec(query);
    if(!ret)
        qDebug() << sqlquery.lastError();
    return ret;
}

QSqlTableModel *Collection::getCollectionModel()
{
    return model;
}

QSqlError Collection::lastError()
{
    return model->lastError();
}

int Collection::getIndexOf(QString key)
{
    return db.record("collection").indexOf(key);
}

QDir Collection::getStorageDir(int costumeId, QString key)
{
    QDir ret = collectionDir;
    ret.mkpath(QString::number(costumeId) + "/" + key); // todo portable ?
    ret.cd(QString::number(costumeId) + "/" + key);
    return ret.absolutePath();
}

void Collection::createStorageDir(int costumeId, QString key)
{
    collectionDir.mkpath(QString::number(costumeId) + "/" + key);
}

int Collection::newCostume()
{
    QSqlRecord r = model->record();
    r.setValue("notdeleted", QVariant(1));
    r.setValue("id", ++lastId);
    model->insertRecord(-1, r);
    model->select();
    return lastId;
}

void Collection::deleteCostumes(QList<int> ids)
{
    QStringList tests;
    foreach(int id, ids)
        tests.push_back("id="+QString::number(id));
    db.exec("UPDATE collection SET notdeleted=0 WHERE " + tests.join(" OR "));
    model->select();
}

QString Collection::getName(int id)
{
    QSqlQuery q(db);
    q.exec("SELECT * FROM collection WHERE id="+QString::number(id));
    if(q.next()) {
        QSqlRecord r = q.record();
        return getName(r);
    }
    return tr("Unknown costume");
}

QString Collection::getName(QSqlRecord rec)
{
    if(rec.value("character").toString() != "")
        return rec.value("character").toString();
    return tr("Unnamed costume");
}

void Collection::InitDefaultInfos()
{
    Costume_info::last_order = 0;
    valid_informations = QMap<QString, Costume_info>();
    valid_informations.insert("id", Costume_info(PK, tr("Id"), true, true));
    valid_informations.insert("notdeleted", Costume_info(Bool, tr("Not Deleted costume")));
    valid_informations.insert("director", Costume_info(ShortString, tr("Piece Director")));
    valid_informations.insert("piece", Costume_info(ShortString, tr("Piece Name")));
    valid_informations.insert("writer", Costume_info(ShortString, tr("Piece Writer")));
    valid_informations.insert("piece_type", Costume_info(ShortString, tr("Piece Type")));
    valid_informations.insert("character", Costume_info(ShortString, tr("Character Name")));
    valid_informations.insert("wearer", Costume_info(ShortString, tr("Worn by : ")));
    valid_informations.insert("year", Costume_info(Number, tr("Year")));
    valid_informations.insert("designer", Costume_info(ShortString, tr("Designer")));
    valid_informations.insert("collection", Costume_info(ShortString, tr("Collection")));
    valid_informations.insert("description", Costume_info(LongString, tr("Description"), false, true));
    //valid_informations.insert("visual", Costume_info(Files, tr("Additional visuals")));

    sql_types.insert(ShortString, "varchar(256)");
    sql_types.insert(LongString, "varchar(4096)");
    sql_types.insert(Number, "integer");
    sql_types.insert(PK, "integer primary key");
    sql_types.insert(Files, "varchar(4096)");
    sql_types.insert(Bool, "integer");
}

QList<QPair<Costume_info, QString> > Collection::sortedValidInformations()
{
    QList<QPair<Costume_info, QString> > orderedInfos;

    foreach(QString key, valid_informations.keys())
        orderedInfos << QPair<Costume_info, QString>(valid_informations.value(key), key);

    qSort(orderedInfos);
    return orderedInfos;
}

QString Collection::keySqlList(QStringList keys)
{
    return keys.join(", ");
}

QString Collection::keyValueList(QStringList keys)
{
    return ":" + keys.join(", :");
}

bool Collection::isValid() const
{
    return valid;
}
