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
    Costume_info( Costume_info_type t, QString n) : type(t), name(n) {order = last_order++;}
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
    static void InitDefaultInfos();
    QVariant getInfo(QString key);
    Costume_info getInfoType(QString key);
    QMap<QString, QVariant> getInfos();
    bool setInfo(QString key, QVariant value);
    static QMap<QString, Costume_info> valid_informations;

    
signals:
    
public slots:

private:

    //QList<Costume_info> informations;
    QMap<QString, QVariant> informations;
};

#endif // COSTUME_H
