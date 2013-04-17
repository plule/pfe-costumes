#include "costume.h"

QMap<QString, Costume_info> Costume::valid_informations;
int Costume_info::last_order;

Costume::Costume(QObject *parent) :
    QObject(parent)
{
}

void Costume::InitDefaultInfos()
{
    Costume_info::last_order = 0;
    valid_informations = QMap<QString, Costume_info>();
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
    valid_informations.insert("visual", Costume_info(Files, tr("Additional visuals")));
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
