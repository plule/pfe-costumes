#include "costume.h"

QMap<QString, Costume_info> Costume::valid_informations;
QMap<Costume_info_type, QString> Costume::sql_types;
int Costume_info::last_order;

Costume::Costume(QObject *parent) : QObject(parent)
{
    informations.insert("id",-1);
}

Costume::Costume(long id, QMap<QString, QVariant> informations, QObject *parent) :
    QObject(parent)
{
    this->informations = informations;
}

void Costume::InitDefaultInfos()
{
    Costume_info::last_order = 0;
    valid_informations = QMap<QString, Costume_info>();
    valid_informations.insert("id", Costume_info(PK, tr("Id")));
    valid_informations.insert("director", Costume_info(ShortString, tr("Piece Director")));
    valid_informations.insert("piece", Costume_info(ShortString, tr("Piece Name")));
    valid_informations.insert("writer", Costume_info(ShortString, tr("Piece Writer")));
    valid_informations.insert("piece_type", Costume_info(ShortString, tr("Piece Type")));
    valid_informations.insert("character", Costume_info(ShortString, tr("Character Name")));
    valid_informations.insert("wearer", Costume_info(ShortString, tr("Worn by : ")));
    valid_informations.insert("year", Costume_info(Number, tr("Year")));
    valid_informations.insert("designer", Costume_info(ShortString, tr("Designer")));
    valid_informations.insert("collection", Costume_info(ShortString, tr("Collection")));
    valid_informations.insert("description", Costume_info(LongString, tr("Description")));
    //valid_informations.insert("visual", Costume_info(Files, tr("Additional visuals")));
    valid_informations.insert("generated_name", Costume_info(LongString, tr("Generated Name")));

    sql_types.insert(ShortString, "varchar(256)");
    sql_types.insert(LongString, "varchar(4096)");
    sql_types.insert(Number, "integer");
    sql_types.insert(PK, "integer primary key");
}

QList<QPair<Costume_info, QString> > Costume::sortedValidInformations()
{
    QList<QPair<Costume_info, QString> > orderedInfos;

    foreach(QString key, valid_informations.keys())
        orderedInfos << QPair<Costume_info, QString>(valid_informations.value(key), key);

    qSort(orderedInfos);
    return orderedInfos;
}

QVariant Costume::getInfo(QString key)
{
    return informations.value(key, QVariant());
}

Costume_info Costume::getInfoType(QString key)
{
    return valid_informations.value(key, Costume_info());
}

QMap<QString, QVariant> Costume::getInfos()
{
    return informations;
}

bool Costume::setInfo(QString key, QVariant value)
{
    if(valid_informations.keys().contains(key)) {
        informations.insert(key, value);
        return true;
    }
    return false;
}

bool Costume::isValid()
{
    foreach(QString key, valid_informations.keys()) {
        if(valid_informations.value(key).mandatory && !informations.contains(key))
            return false;
    }
    return true;
}

QString Costume::toString()
{
    QString ret = "Costume " + QString::number(getId()) + "\n";
    foreach(QString key, informations.keys()) {
        Costume_info info = valid_informations.value(key);
        ret += "  " + key + ":" + informations.value(key).toString() + "\n";
    }
    return ret;
}

int Costume::getId()
{
    return informations.value("id",-1).toInt();
}

void Costume::setId(int id)
{
    informations.insert("id",id);
}

QString Costume::getName()
{
    return tr("%1 in %2").arg(getInfo("character").toString(), getInfo("piece").toString());
}
