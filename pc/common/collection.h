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
public:
    explicit Collection(QObject *parent = 0);
    Collection(QObject *parent = 0, QString collectionPath = "");
    ~Collection();
    QSqlTableModel* getCollectionModel();
    QSqlError lastError();
    int getIndexOf(QString key);
    QDir getStorageDir(int costumeId, QString key);
    void createStorageDir(int costumeId, QString key);
    int newCostume();
    void deleteCostume(int id);
    QString getName(QSqlRecord rec);
    QString getName(int id);
    QStringList *getExistings(QString key);
    void loadExistings();
    bool isDirty();
    bool submit();
    void revert();
    int getRow(int id);
    QSqlRecord getRecord(int id);

    static void InitDefaultInfos();
    static QMap<QString, Costume_info> valid_informations;
    static QList<QPair<Costume_info, QString> > sortedValidInformations();
    static QMap<Costume_info_type, QString> sql_types;
    
    bool isValid() const;

signals:
    
public slots:

private:
    bool createCollectionTable();
    QString keySqlList(QStringList keys);
    QString keyValueList(QStringList keys);

    QSqlDatabase db;
    QSqlTableModel *model;
    QDir collectionDir;
    QMap<QString,QStringList* > existing;
    bool valid;
    int lastId;
    
};

#endif // COLLECTIONMANAGER_H
