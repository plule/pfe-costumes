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
#include <QDirIterator>
#include "common/uniqueproxymodel.h"

/**
 * @brief The Costume_info_type enum describes different
 * kind of datas that can be stored in the database
 */
enum Costume_info_type {
    Invalid,
    PK,
    ShortString,
    LongString,
    Number,
    Files,
    Bool
};

/**
 * @brief The Costume_info struct contains all the common infos of ONE costume parameter
 */
struct Costume_info {
    Costume_info_type type;
    QString name;
    int order;
    static int last_order;
    bool autocomplete;
    bool visible;
    bool external;
    Costume_info( Costume_info_type type, QString name, bool autocomplete=true, bool visible=true, bool external=false) :
        type(type), name(name), autocomplete(autocomplete), visible(visible),external(external) {order = last_order++;}
    Costume_info() {type = Invalid;}
    friend bool operator<(const Costume_info& left, const Costume_info& right){
        return left.order < right.order;
    }
};

/**
 * @brief The Collection class allow managing a collection of costumes
 */
class Collection : public QObject
{
    Q_OBJECT
    Q_ENUMS(Costume_info_type)

public:
    explicit Collection(QObject *parent = 0);
    Collection(QObject *parent = 0, QString collectionPath = "");
    ~Collection();

    /**
     * @brief getCollectionModel
     * @return The model of the Sql Table. This can be useful for visualising datas
     */
    QSqlTableModel* getCollectionModel();

    /**
     * @brief lastError
     * @return The last error encountered by the database
     */
    QSqlError lastError();

    /**
     * @brief getStorageDir
     * @param costumeId
     * @param key
     * @return Path to a dir where datas for a specific key are stored.
     */
    QDir getStorageDir(int costumeId, QString key);

    /**
     * @brief getTempStorageDir
     * @param costumeId
     * @param key
     * @return Path a temporary storage dir where datas can be saved. When submit() is called
     * the files put in this folder are copied to the storage dir. When revert() is called, they
     * are deleted
     */
    QDir getTempStorageDir(int costumeId, QString key);

    /**
     * @brief getAllDirs
     * @param costumeId
     * @param key
     * @return All the dirs of a key (storage and temp)
     */
    QList<QDir> getAllDirs(int costumeId, QString key);

    /**
     * @brief getNewFilePath
     * @param costumeId
     * @param key
     * @param extension
     * @return A new file of type "extension" located in the temp folder that does
     * not exists in the temp or the storage dirs.
     */
    QString getNewFilePath(int costumeId, QString key, QString extension);

    /**
     * @brief fileExists
     * @param costumeId
     * @param key
     * @param filename
     * @return Check if a file exists in the temp dir or the storage dir
     */
    bool fileExists(int costumeId, QString key, QString filename);

    /**
     * @brief createStorageDir ensures that the storage dir exists
     * @param costumeId
     * @param key
     */
    void createStorageDir(int costumeId, QString key);

    /**
     * @brief newCostume creates a new costume in the database
     * @return its id
     */
    int newCostume();

    /**
     * @brief deleteCostume deletes a costume from the database
     * @param id
     */
    void deleteCostume(int id);

    /**
     * @brief getName
     * @param rec
     * @return The name of a costume described by a sql line
     */
    QString getName(QSqlRecord rec);

    /**
     * @brief getName
     * @param id
     * @return The name of a costume from its id
     */
    QString getName(int id);

    /**
     * @brief getCompleter
     * @param key
     * @return A completer object that works for a particular key
     */
    QCompleter *getCompleter(QString key);

    /**
     * @brief loadCompleters creates all the completers
     */
    void loadCompleters();

    /**
     * @brief isDirty check if unsaved changed are present in the database
     * @return
     */
    bool isDirty();

    /**
     * @brief submit saves the changes to the database
     * @return
     */
    bool submit();

    /**
     * @brief revert cancels «dirty» changes, remove temp files
     */
    void revert();

    /**
     * @brief select
     * @return rerun selection on the model
     */
    bool select();

    /**
     * @brief cleanUp cleans the temp storage
     */
    void cleanUp();

    /**
     * @brief getRow returns the row of a costume in the database
     * @param id
     * @return
     */
    int getRow(int id);

    /**
     * @brief getRecord return a sql record of a costume
     * @param id
     * @return
     */
    QSqlRecord getRecord(int id);

    /**
     * @brief isValid
     * @return validty of the costume
     */
    bool isValid() const;
    QList<QPair<Costume_info, QString> > sortedContent();

    static QMap<Costume_info_type, QString> sql_types;
    static QMap<Costume_info_type, QString> InitSqlTypes();
    

signals:
    /**
     * @brief synchronised is emitted after save
     */
    void synchronised();

public slots:

private:
    QSqlTableModel *loadContent(QSqlDatabase *m_db);
    QString keySqlList(QStringList keys);
    QString keyValueList(QStringList keys);
    QString collectionPath;

    QSqlDatabase *m_db;
    QSqlTableModel *m_model;
    QDir m_collectionDir;
    QDir m_tempDir;
    QMap<QString,QCompleter* > m_completers;
    bool m_valid;
    int m_lastId;
    QMap<QString, Costume_info> m_content;
};

#endif // COLLECTIONMANAGER_H
