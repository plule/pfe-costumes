#ifndef COLLECTIONMANAGER_H
#define COLLECTIONMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDesktopServices>
#include <QDir>
#include <QDebug>
#include <QStringList>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

enum Costume_info_type {
    Invalid,
    PK,
    ShortString,
    LongString,
    Number,
    Files
};

struct Costume_info {
    Costume_info_type type;
    QString name;
    int order;
    static int last_order;
    bool mandatory;
    bool visible;
    bool external;
    Costume_info( Costume_info_type t, QString n, bool m=false, bool v=true, bool e=false) : type(t), name(n),mandatory(m),visible(v),external(e) {order = last_order++;}
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

    static void InitDefaultInfos();
    static QMap<QString, Costume_info> valid_informations;
    static QList<QPair<Costume_info, QString> > sortedValidInformations();
    static QMap<Costume_info_type, QString> sql_types;
    
    bool isValid() const;

signals:
    
public slots:
    void prepareRecord(QSqlRecord & record);
    void prepareRecord(int row, QSqlRecord & record);

private:
    bool createCollectionTable();
    QString keySqlList(QStringList keys);
    QString keyValueList(QStringList keys);

    QSqlDatabase db;
    QSqlTableModel *model;
    bool valid;
    
};

#endif // COLLECTIONMANAGER_H
