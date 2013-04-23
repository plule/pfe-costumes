#include "collectionmanager.h"

QMap<QString, Costume_info> CollectionManager::valid_informations;
QMap<Costume_info_type, QString> CollectionManager::sql_types;
int Costume_info::last_order;

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
    db.setDatabaseName(collectionPath);
    collection->setTable("collection");
    connect(collection, SIGNAL(beforeInsert(QSqlRecord&)), this, SLOT(prepareRecord(QSqlRecord&)));
    connect(collection, SIGNAL(beforeUpdate(int,QSqlRecord&)), this, SLOT(prepareRecord(int,QSqlRecord&)));
    return db.open();
}

bool CollectionManager::createCollectionTable()
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
    qDebug() << query;
    bool ret = sqlquery.exec(query);
    if(!ret)
        qDebug() << sqlquery.lastError();
    return false;
}

QSqlTableModel *CollectionManager::getCollectionModel()
{
    return collection;
}

QSqlError CollectionManager::lastError()
{
    return collection->lastError();
}

int CollectionManager::getIndexOf(QString key)
{
    return db.record("collection").indexOf(key);
}

void CollectionManager::InitDefaultInfos()
{
    Costume_info::last_order = 0;
    valid_informations = QMap<QString, Costume_info>();
    valid_informations.insert("id", Costume_info(PK, tr("Id")));
    valid_informations.insert("director", Costume_info(ShortString, tr("Piece Director")));
    valid_informations.insert("piece", Costume_info(ShortString, tr("Piece Name")));
    valid_informations.insert("writer", Costume_info(ShortString, tr("Piece Writer")));
    valid_informations.insert("piece_type", Costume_info(ShortString, tr("Piece Type")));
    valid_informations.insert("character", Costume_info(ShortString, tr("Character Name")));
    valid_informations.insert("wearer", Costume_info(ShortString, tr("Worn by : ")));
    valid_informations.insert("year", Costume_info(Number, tr("Year")));
    valid_informations.insert("designer", Costume_info(ShortString, tr("Designer")));
    valid_informations.insert("collection", Costume_info(ShortString, tr("Collection")));
    valid_informations.insert("description", Costume_info(LongString, tr("Description")));
    //valid_informations.insert("visual", Costume_info(Files, tr("Additional visuals")));
    valid_informations.insert("generated_name", Costume_info(LongString, tr("Generated Name")));

    sql_types.insert(ShortString, "varchar(256)");
    sql_types.insert(LongString, "varchar(4096)");
    sql_types.insert(Number, "integer");
    sql_types.insert(PK, "integer primary key");
}

QList<QPair<Costume_info, QString> > CollectionManager::sortedValidInformations()
{
    QList<QPair<Costume_info, QString> > orderedInfos;

    foreach(QString key, valid_informations.keys())
        orderedInfos << QPair<Costume_info, QString>(valid_informations.value(key), key);

    qSort(orderedInfos);
    return orderedInfos;
}

void CollectionManager::prepareRecord(QSqlRecord &record)
{
    QString character = record.value("character").toString();
    QString scene = record.value("piece").toString();
    record.setValue(record.indexOf("generated_name"), tr("%1 in %2").arg(character, scene));

    QSqlField nameField("generated_name", QVariant::String);
    nameField.setValue(tr("%1 in %2").arg(character, scene));
    record.append(nameField);
}

void CollectionManager::prepareRecord(int row, QSqlRecord &record)
{
    (void)row;
    prepareRecord(record);
}

QString CollectionManager::keySqlList(QStringList keys)
{
    return keys.join(", ");
}

QString CollectionManager::keyValueList(QStringList keys)
{
    return ":" + keys.join(", :");
}
