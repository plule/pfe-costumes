#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QList>
#include <QMetaType>
#include "qextserialport.h"
#include "../../interfaces/interfaces.h"

struct ArduinoMessage
{
    MSG_TYPE type;
    ard_id_t id;
    uint8_t expe;
    uint8_t dest;
    QString data;

    QString toString() {
        QString ret = "from %1 to %2 : (%3) %4";
        return ret.arg(QString::number(id), QString::number(expe), QString::number(type), data);
    }
};


struct Arduino
{
    ard_id_t id;
    ARD_ROLE role;
};

Q_DECLARE_METATYPE(Arduino)

class Morphology : public QObject
{
    Q_OBJECT
public:
    explicit Morphology(QObject *parent = 0);
    Morphology(QString name, QObject *parent = 0);


signals:
    void arduinoListUpdate(QList<Arduino> arduinos);
    
public slots:
    void sendHelloMessage();
    void setMicrosecond(int ms);
    
private slots:
    void onDataAvailable();

private:
    void handleMessage(QString message);
    void handleMessage(ArduinoMessage message);
    void sendMessage(MSG_TYPE type, int id, int dest, int data);

    QextSerialPort *m_port;
    QString message_part;
    QList<Arduino> arduinos;
};

#endif // MORPHOLOGY_H
