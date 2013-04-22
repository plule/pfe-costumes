#ifndef COSTUME_H
#define COSTUME_H

#include <QObject>
#include <QMap>
#include <QVariant>

enum Costume_info_type {
    Invalid,
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
    Costume_info( Costume_info_type t, QString n, bool m=false) : type(t), name(n),mandatory(m) {order = last_order++;}
    Costume_info() {type = Invalid;}
    friend bool operator<(const Costume_info& left, const Costume_info& right){
        return left.order < right.order;
    }
};

class Costume : public QObject
{
    Q_OBJECT

public:
    explicit Costume(QObject *parent = 0);
    Costume(long id, QMap<QString, QVariant> informations, QObject *parent = 0);

    QVariant getInfo(QString key);
    Costume_info getInfoType(QString key);
    QMap<QString, QVariant> getInfos();
    bool setInfo(QString key, QVariant value);
    bool isValid();
    QString toString();
    int getId();
    void setId(int id);
    QString getName();

    static void InitDefaultInfos();
    static QMap<QString, Costume_info> valid_informations;
    static QMap<Costume_info_type, QString> sql_types;

    
signals:
    
public slots:

private:

    //QList<Costume_info> informations;
    QMap<QString, QVariant> informations;
};

#endif // COSTUME_H
