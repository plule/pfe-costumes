#include "costume.h"

const QList<Costume_info> Costume::default_infos = QList<Costume_info>()
        << Costume_info("director", ShortString, tr("Director"))
        << Costume_info("piece", ShortString, tr("Piece"))
        << Costume_info("character", ShortString, tr("Character"))
        << Costume_info("wearer", ShortString, tr("Wearer"))
        << Costume_info("year", Number, tr("Year"))
        << Costume_info("designer", ShortString, tr("Designer"))
        << Costume_info("collection", ShortString, tr("Collection"))
        << Costume_info("description", LongString, tr("Description"))
        << Costume_info("piece_type", ShortString, tr("Piece Type"))
        << Costume_info("writer", ShortString, tr("Writer"))
           ;

Costume::Costume(QObject *parent) :
    QObject(parent)
{
    informations = QList<Costume_info>(default_infos);
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
