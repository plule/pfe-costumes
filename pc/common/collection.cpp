#include "collection.h"

QMap<Costume_info_type, QString> Collection::sql_types = Collection::InitSqlTypes();
int Costume_info::last_order;

Collection::Collection(QObject *parent) :
    QObject(parent)
{
    valid = false;
}

Collection::Collection(QObject *parent, QString collectionPath) : QObject(parent)
{
    valid = false;
    this->collectionPath = collectionPath;
    /* Connect to the db */
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", collectionPath));
    db->setDatabaseName(collectionPath);
    if(!db->open())
        return;

    /* Load meta content and do the validation */
    model = loadContent(db);
    if(model == 0)
        return;

    /* Find highest unused id */
    QSqlQuery q(*db);
    q.exec("SELECT MAX(id) FROM collection");
    lastId = 0;
    if(q.next()) {
        lastId = q.value(0).toInt();
    }

    /* Load autocompleters */
    loadCompleters();

    /* Init personnal storage path */
    QFileInfo coll(collectionPath);
    collectionDir = coll.absoluteDir();
    collectionDir.mkdir(coll.baseName()+"_FILES");
    collectionDir.cd(coll.baseName()+"_FILES");

    valid = true;

    /* Init temp storage path */
    tempDir = QDir::temp();
    tempDir.mkdir(coll.baseName()+"_FILES");
    tempDir.cd(coll.baseName()+"_FILES");
}

QSqlTableModel *Collection::loadContent(QSqlDatabase *db)
{
    if(!db->tables().contains("collection") || !db->tables().contains("content")) {
        return 0;
    }
    QSqlTableModel *model = new QSqlTableModel(this, *db);

    // TODO : validation
    Costume_info::last_order = 0;
    content = QMap<QString, Costume_info>();
    /* System infos */
    content.insert("id", Costume_info(PK, tr("Id"), true));
    content.insert("notdeleted", Costume_info(Bool, tr("Not Deleted costume"), false));

    /* User infos */
    QSqlQuery q(*db);
    q.exec("SELECT * FROM content");
    QSqlRecord r = q.record();
    int ikey = r.indexOf("key");
    int iname = r.indexOf("name");
    int itype = r.indexOf("type");
    int iautocomplete = r.indexOf("autocomplete");
    int ivisible = r.indexOf("visible");
    while(q.next()) {
        QSqlRecord r = q.record();
        QString key = r.value(ikey).toString();
        QString type_s = r.value(itype).toString();
        QString name = r.value(iname).toString();
        bool autocomplete = r.value(iautocomplete).toBool(); // TODO
        bool visible = r.value(ivisible).toBool();
        Costume_info_type type = Invalid;
        if(type_s == "ShortString")
            type = ShortString;
        if(type_s == "Number")
            type = Number;
        if(type_s == "LongString")
            type = LongString;
        if(type_s == "Bool")
            type = Bool;
        if(type_s == "Files")
            type = Files;
        content.insert(key, Costume_info(type, name, visible));
    }
    model->setTable("collection");
    model->setFilter("notdeleted == 1");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    return model;
}

Collection::~Collection()
{
    db->close();
    delete db;
    delete model;
    QSqlDatabase::removeDatabase(collectionPath);
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
    return db->record("collection").indexOf(key);
}

QDir Collection::getStorageDir(int costumeId, QString key)
{
    QDir ret = collectionDir;
    ret.mkpath(QString::number(costumeId) + "/" + key); // todo portable ?
    ret.cd(QString::number(costumeId) + "/" + key);
    return ret.absolutePath();
}

QDir Collection::getTempStorageDir(int costumeId, QString key)
{
    QDir ret = tempDir;
    ret.mkpath(QString::number(costumeId) + "/" + key); // todo portable ?
    ret.cd(QString::number(costumeId) + "/" + key);
    return ret.absolutePath();
}

QList<QDir> Collection::getAllDirs(int costumeId, QString key)
{
    return QList<QDir>() << getTempStorageDir(costumeId, key) << getStorageDir(costumeId, key);
}

QString Collection::getNewFilePath(int costumeId, QString key,QString extension)
{
    int i=1;
    while(i < 10000) { // TODO while(1) ?
        QString filename = QString("%1.%2").arg(QString::number(i), 3, QLatin1Char('0')).arg(extension);
        if(!fileExists(costumeId, key, filename)) {
            QString path = getTempStorageDir(costumeId, key).absoluteFilePath(filename);
            QFile f(path);
            f.open( QIODevice::WriteOnly ); // ensure creation
            f.close();
            return path;
        }
        i++;
    }
    return "";
}

bool Collection::fileExists(int costumeId, QString key, QString filename)
{
    QDir tempDir = getTempStorageDir(costumeId, key);
    QDir storDir = getStorageDir(costumeId, key);
    return (QFile::exists(tempDir.absoluteFilePath(filename)) || QFile::exists(storDir.absoluteFilePath(filename)));
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
    if(model->isDirty())
        return true;
    // Recursive find new pics in temp dir
    QDirIterator it(tempDir.absolutePath(), QDir::Files, QDirIterator::Subdirectories);
    return it.hasNext();
}

bool Collection::submit()
{
    bool ret = model->submitAll();
    QDirIterator it(tempDir.absolutePath(), QDir::Files, QDirIterator::Subdirectories);
    while(it.hasNext()) {
        it.next();
        QString filename = it.filePath();
        filename.replace(tempDir.absolutePath()+QDir::separator(),"");
        QString src = tempDir.absoluteFilePath(filename);
        QString dest = collectionDir.absoluteFilePath(filename);
        QFile::remove(dest);
        QFile::copy(src, dest);
        QFile::remove(src);
    }
    if(ret)
        emit synchronised();
    else
        qDebug() << model->lastError();
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

QList<QPair<Costume_info, QString> > Collection::sortedContent()
{
    QList<QPair<Costume_info, QString> > orderedInfos;

    foreach(QString key, content.keys())
        orderedInfos << QPair<Costume_info, QString>(content.value(key), key);

    qSort(orderedInfos);
    return orderedInfos;
}

QMap<Costume_info_type, QString> Collection::InitSqlTypes()
{
    QMap<Costume_info_type, QString> sql_types = QMap<Costume_info_type, QString>();
    sql_types.insert(ShortString, "varchar(256)");
    sql_types.insert(LongString, "varchar(4096)");
    sql_types.insert(Number, "integer");
    sql_types.insert(PK, "integer primary key");
    sql_types.insert(Files, "varchar(4096)");
    sql_types.insert(Bool, "integer");
    return sql_types;
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
