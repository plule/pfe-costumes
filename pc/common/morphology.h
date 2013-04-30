#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include <QObject>
#include <QDebug>
#include "qextserialport.h"

class Morphology : public QObject
{
    Q_OBJECT
public:
    explicit Morphology(QObject *parent = 0);
    Morphology(QString name, QObject *parent = 0);


signals:
    
public slots:
    void sendHelloMessage();
    
private slots:
    void onDataAvailable();

private:
    QextSerialPort *m_port;
    QByteArray message;
};

#endif // MORPHOLOGY_H
