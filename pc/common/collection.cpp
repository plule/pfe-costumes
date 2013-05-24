#include "collection.h"

QMap<Costume_info_type, QString> Collection::sql_types = Collection::InitSqlTypes();
int Costume_info::last_order;

Collection::Collection(QObject *parent) :
    QObject(parent)
{
    m_valid = false;
}

Collection::Collection(QObject *parent, QString collectionPath) : QObject(parent)
{
    m_valid = false;
    this->collectionPath = collectionPath;
    /* Connect to the db */
    m_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", collectionPath));
    m_db->setDatabaseName(collectionPath);
    if(!m_db->open())
        return;

    /* Load meta content and do the validation */
    m_model = loadContent(m_db);
    if(m_model == 0)
        return;

    /* Find highest unused id */
    QSqlQuery q(*m_db);
    q.exec("SELECT MAX(id) FROM collection");
    m_lastId = 0;
    if(q.next()) {
        m_lastId = q.value(0).toInt();
    }

    /* Load autocompleters */
    loadCompleters();

    /* Init personnal storage path */
    QFileInfo coll(collectionPath);
    m_collectionDir = coll.absoluteDir();
    m_collectionDir.mkdir(coll.baseName()+"_FILES");
    m_collectionDir.cd(coll.baseName()+"_FILES");

    m_valid = true;

    /* Init temp storage path */
    m_tempDir = QDir::temp();
    m_tempDir.mkdir(coll.baseName()+"_FILES");
    m_tempDir.cd(coll.baseName()+"_FILES");
}

QSqlTableModel *Collection::loadContent(QSqlDatabase *db)
{
    if(!db->tables().contains("collection") || !db->tables().contains("content")) {
        return 0;
    }
    QSqlTableModel *model = new QSqlTableModel(this, *db);

    // TODO : validation
    Costume_info::last_order = 0;
    m_content = QMap<QString, Costume_info>();
    /* System infos */
    m_content.insert("id", Costume_info(PK, tr("Id"), true));
    m_content.insert("notdeleted", Costume_info(Bool, tr("Not Deleted costume"), false));

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
        bool autocomplete = r.value(iautocomplete).toBool();
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
        m_content.insert(key, Costume_info(type, name, autocomplete, visible));
    }
    model->setTable("collection");
    model->setFilter("notdeleted == 1");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    return model;
}

Collection::~Collection()
{
    m_db->close();
    delete m_db;
    delete m_model;
    QSqlDatabase::removeDatabase(collectionPath);
}

QSqlTableModel *Collection::getCollectionModel()
{
    return m_model;
}

QSqlError Collection::lastError()
{
    return m_model->lastError();
}

QDir Collection::getStorageDir(int costumeId, QString key)
{
    QDir ret = m_collectionDir;
    ret.mkpath(QString::number(costumeId) + "/" + key); // todo portable ?
    ret.cd(QString::number(costumeId) + "/" + key);
    return ret.absolutePath();
}

QDir Collection::getTempStorageDir(int costumeId, QString key)
{
    QDir ret = m_tempDir;
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
    m_collectionDir.mkpath(QString::number(costumeId) + "/" + key);
}

int Collection::newCostume()
{
    QSqlRecord r = m_model->record();
    r.setValue("notdeleted", QVariant(1));
    r.setValue("id", ++m_lastId);
    m_model->insertRecord(-1, r);
    return m_lastId;
}

void Collection::deleteCostume(int id)
{
    int row = getRow(id);
    QSqlRecord rec = m_model->record(row);
    rec.setValue("notdeleted",0);
    m_model->setRecord(row, rec);
}

QString Collection::getName(int id)
{
    return getName(getRecord(id));
}

QCompleter *Collection::getCompleter(QString key)
{
    return m_completers.value(key);
}

void Collection::loadCompleters()
{
    foreach(QString key, m_content.keys()) {
        if(m_content.value(key).autocomplete) {
            int column = m_model->record().indexOf(key);
            UniqueProxyModel *proxy = new UniqueProxyModel(column, this);
            proxy->setSourceModel(m_model);
            QCompleter *c = new QCompleter(proxy,this);
            c->setCompletionColumn(m_model->record().indexOf(key));
            m_completers.insert(key, c);
        }
    }
}

bool Collection::isDirty()
{
    if(m_model->isDirty())
        return true;
    // Recursive find new pics in temp dir
    QDirIterator it(m_tempDir.absolutePath(), QDir::Files, QDirIterator::Subdirectories);
    return it.hasNext();
}

bool Collection::submit()
{
    bool ret = m_model->submitAll();
    QDirIterator it(m_tempDir.absolutePath(), QDir::Files, QDirIterator::Subdirectories);
    while(it.hasNext()) {
        it.next();
        QString filename = it.filePath();
        filename.replace(m_tempDir.absolutePath()+QDir::separator(),"");
        QString src = m_tempDir.absoluteFilePath(filename);
        QString dest = m_collectionDir.absoluteFilePath(filename);
        QFile::remove(dest);
        QFile::copy(src, dest);
        QFile::remove(src);
    }
    if(ret)
        emit synchronised();
    else
        qDebug() << m_model->lastError();
    return ret;
}

void Collection::revert()
{
    m_model->revertAll();
    cleanUp();
}

bool Collection::select()
{
    bool ret = m_model->select();
    if(ret)
        emit synchronised();
    return ret;
}

void Collection::cleanUp()
{
    QDirIterator it(m_tempDir.absolutePath(), QDir::Files, QDirIterator::Subdirectories);
    while(it.hasNext())
        QFile::remove(it.next());
}

int Collection::getRow(int id)
{
    for(int row=0; row < m_model->rowCount(); ++row)
        if ( m_model->index(row, 0).data(Qt::DisplayRole).toInt() == id )
            return row;
    return -1;
}

QSqlRecord Collection::getRecord(int id)
{
    return m_model->record(getRow(id));
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

    foreach(QString key, m_content.keys())
        orderedInfos << QPair<Costume_info, QString>(m_content.value(key), key);

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
    return m_valid;
}
