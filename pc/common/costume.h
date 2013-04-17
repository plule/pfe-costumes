#ifndef COSTUME_H
#define COSTUME_H

#include <QObject>
#include <QVariant>

enum Costume_info_type {
    Invalid,
    ShortString,
    LongString,
    Number
};

struct Costume_info {
    QString key;
    Costume_info_type type;
    QString name;
    QVariant value;
    Costume_info( QString k, Costume_info_type t, QString n, QVariant v ) : key(k), type(t), name(n), value(v) {}
    Costume_info( QString k, Costume_info_type t, QString n) : key(k), type(t), name(n) {}
    Costume_info() {type = Invalid;}
};

class Costume : public QObject
{
    Q_OBJECT

public:
    explicit Costume(QObject *parent = 0);
    static void InitDefaultInfos();
    QVariant getInfo(QString key);
    Costume_info getCompleteInfo(QString key);
    Costume_info getCompleteDefaultInfo(QString key);
    QList<Costume_info> getInformations();
    bool setInformation(QString key, QVariant value);
    static QList<Costume_info> default_infos;

    
signals:
    
public slots:

private:
    QList<Costume_info> informations;
};

#endif // COSTUME_H
