#include "collection.h"

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
        loadContent();
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
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();
    loadCompleters();
    QFileInfo coll(collectionPath);
    collectionDir = coll.absoluteDir();
    collectionDir.mkdir(coll.baseName()+"_FILES");
    collectionDir.cd(coll.baseName()+"_FILES");
}

Collection::~Collection()
{
    db.close();
}

bool Collection::createCollectionTable()
{
    QString query = "create table collection (";
    QSqlQuery sqlquery;
    QList<QPair<Costume_info, QString> > orderedInfos = sortedContent();
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
    return lastId;
}

void Collection::deleteCostume(int id)
{
    int row = getRow(id);
    QSqlRecord rec = model->record(row);
    rec.setValue("notdeleted",0);
    model->setRecord(row, rec);
}

QString Collection::getName(int id)
{
    return getName(getRecord(id));
}

QCompleter *Collection::getCompleter(QString key)
{
    return completers.value(key);
}

void Collection::loadCompleters()
{
    foreach(QString key, content.keys()) {
        int column = model->record().indexOf(key);
        UniqueProxyModel *proxy = new UniqueProxyModel(column, this);
        proxy->setSourceModel(model);
        QCompleter *c = new QCompleter(proxy,this);
        c->setCompletionColumn(model->record().indexOf(key));
        completers.insert(key, c);
    }
}

bool Collection::isDirty()
{
    return model->isDirty();
}

bool Collection::submit()
{
    bool ret = model->submitAll();
    if(ret)
        emit synchronised();
    return ret;
}

void Collection::revert()
{
    model->revertAll();
}

bool Collection::select()
{
    bool ret = model->select();
    if(ret)
        emit synchronised();
    return ret;
}

int Collection::getRow(int id)
{
    for(int row=0; row < model->rowCount(); ++row)
        if ( model->index(row, 0).data(Qt::DisplayRole).toInt() == id )
            return row;
    return -1;
}

QSqlRecord Collection::getRecord(int id)
{
    return model->record(getRow(id));
}

QString Collection::getName(QSqlRecord rec)
{
    if(rec.value("character").toString() != "")
        return rec.value("character").toString();
    return tr("Unnamed costume");
}

void Collection::loadContent()
{
    Costume_info::last_order = 0;
    content = QMap<QString, Costume_info>();
    content.insert("id", Costume_info(PK, tr("Id"), true));
    content.insert("notdeleted", Costume_info(Bool, tr("Not Deleted costume"), false));
    content.insert("director", Costume_info(ShortString, tr("Piece Director")));
    content.insert("piece", Costume_info(ShortString, tr("Piece Name")));
    content.insert("writer", Costume_info(ShortString, tr("Piece Writer")));
    content.insert("piece_type", Costume_info(ShortString, tr("Piece Type")));
    content.insert("character", Costume_info(ShortString, tr("Character Name")));
    content.insert("wearer", Costume_info(ShortString, tr("Worn by : ")));
    content.insert("year", Costume_info(Number, tr("Year")));
    content.insert("designer", Costume_info(ShortString, tr("Designer")));
    content.insert("collection", Costume_info(ShortString, tr("Collection")));
    content.insert("description", Costume_info(LongString, tr("Description")));
    //valid_informations.insert("visual", Costume_info(Files, tr("Additional visuals")));


}

QList<QPair<Costume_info, QString> > Collection::sortedContent()
{
    QList<QPair<Costume_info, QString> > orderedInfos;

    foreach(QString key, content.keys())
        orderedInfos << QPair<Costume_info, QString>(content.value(key), key);

    qSort(orderedInfos);
    return orderedInfos;
}

void Collection::InitSqlTypes()
{
    sql_types = QMap<Costume_info_type, QString>();
    sql_types.insert(ShortString, "varchar(256)");
    sql_types.insert(LongString, "varchar(4096)");
    sql_types.insert(Number, "integer");
    sql_types.insert(PK, "integer primary key");
    sql_types.insert(Files, "varchar(4096)");
    sql_types.insert(Bool, "integer");
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
