#ifndef COLLECTIONMANAGER_H
#define COLLECTIONMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QDebug>
#include <QStringList>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QFileInfo>
#include <QList>
#include <QMap>
#include <QSet>
#include <QCompleter>
#include <QStringListModel>
#include "common/uniqueproxymodel.h"

enum Costume_info_type {
    Invalid,
    PK,
    ShortString,
    LongString,
    Number,
    Files,
    Bool
};

struct Costume_info {
    Costume_info_type type;
    QString name;
    int order;
    static int last_order;
    bool visible;
    bool external;
    Costume_info( Costume_info_type t, QString n, bool v=true, bool e=false) : type(t), name(n),visible(v),external(e) {order = last_order++;}
    Costume_info() {type = Invalid;}
    friend bool operator<(const Costume_info& left, const Costume_info& right){
        return left.order < right.order;
    }
};

class Collection : public QObject
{
    Q_OBJECT
    Q_ENUMS(Costume_info_type)

public:
    explicit Collection(QObject *parent = 0);
    Collection(QObject *parent = 0, QString collectionPath = "");
    ~Collection();

    QSqlTableModel* getCollectionModel();
    QSqlError lastError();
    int getIndexOf(QString key);
    QDir getStorageDir(int costumeId, QString key);
    QDir getTempStorageDir(int costumeId, QString key);
    QString getNewFilePath(int costumeId, QString key);
    bool fileExists(int costumeId, QString key, QString filename);
    void createStorageDir(int costumeId, QString key);
    int newCostume();
    void deleteCostume(int id);
    QString getName(QSqlRecord rec);
    QString getName(int id);
    QCompleter *getCompleter(QString key);
    void loadCompleters();
    bool isDirty();
    bool submit();
    void revert();
    bool select();
    int getRow(int id);
    QSqlRecord getRecord(int id);
    bool isValid() const;

    QList<QPair<Costume_info, QString> > sortedContent();

    static QMap<Costume_info_type, QString> sql_types;
    static QMap<Costume_info_type, QString> InitSqlTypes();
    

signals:
    void synchronised();

public slots:

private:
    QSqlTableModel *loadContent(QSqlDatabase db);
    QString keySqlList(QStringList keys);
    QString keyValueList(QStringList keys);
    QString collectionPath;

    QSqlDatabase db;
    QSqlTableModel *model;
    QDir collectionDir;
    QDir tempDir;
    QMap<QString,QCompleter* > completers;
    bool valid;
    int lastId;
    QMap<QString, Costume_info> content;
    
};

#endif // COLLECTIONMANAGER_H
