#include "costume.h"

QList<Costume_info> Costume::default_infos = QList<Costume_info>();

Costume::Costume(QObject *parent) :
    QObject(parent)
{
    informations = QList<Costume_info>(default_infos);
}

void Costume::InitDefaultInfos()
{
    default_infos = QList<Costume_info>()
            << Costume_info("director", ShortString, tr("Piece Director"))
            << Costume_info("piece", ShortString, tr("Piece Name"))
            << Costume_info("writer", ShortString, tr("Piece Writer"))
            << Costume_info("piece_type", ShortString, tr("Piece Type"))
            << Costume_info("character", ShortString, tr("Character Name"))
            << Costume_info("wearer", ShortString, tr("Worn by : "))
            << Costume_info("year", Number, tr("Year"))
            << Costume_info("designer", ShortString, tr("Designer"))
            << Costume_info("collection", ShortString, tr("Collection"))
            << Costume_info("description", LongString, tr("Description"))
               ;
}

QVariant Costume::getInfo(QString key)
{
    foreach(Costume_info info, informations) {
        if(key == info.key)
            return info.value;
    }
    return QVariant();
}

Costume_info Costume::getCompleteInfo(QString key)
{
    foreach(Costume_info info, informations) {
        if(key == info.key)
            return info;
    }
    return Costume_info();
}

Costume_info Costume::getCompleteDefaultInfo(QString key)
{
    foreach(Costume_info info, default_infos) {
        if(key == info.key)
            return info;
    }
    return Costume_info();
}

QList<Costume_info> Costume::getInformations()
{
    return informations;
}

bool Costume::setInformation(QString key, QVariant value)
{
    foreach(Costume_info info, informations) {
        if(key == info.key) {
            info.value = value;
            return true;
        }
    }
    return false;
}
