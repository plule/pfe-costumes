#include "morphology.h"

Morphology::Morphology(QObject *parent) :
    QObject(parent)
{

}

Morphology::Morphology(QString name, QObject *parent) : QObject(parent)
{
    m_port = new QextSerialPort(name);
    connect(m_port, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
    m_port->open(QIODevice::ReadWrite);
}

void Morphology::sendHelloMessage()
{
    arduinos.clear();
    emit(arduinoListUpdate(arduinos));
    sendMessage(DISCOVER, 0, 0, 0);
}

void Morphology::setMicrosecond(int ms)
{
    sendMessage(COMMAND, 42, 3, ms);
}

void Morphology::onDataAvailable()
{
    message_part.append(m_port->readAll());
    QStringList messages = message_part.split(MSG_SEP);
    message_part = messages.takeLast(); // The last message is incomplete
    foreach(QString message, messages)
        handleMessage(message);
}

void Morphology::handleMessage(QString message)
{
    QStringList spl = message.split(ARG_SEP);
    if(spl.size() != ARG_NUMBER)
        return;
    ArduinoMessage msg;

    if(spl.at(ARG_DEST).toInt() == ARD_MASTER) {
        msg.type = (MSG_TYPE)spl.at(ARG_TYPE).toInt();
        msg.id = spl.at(ARG_ID).toInt();
        msg.expe = spl.at(ARG_EXPE).toInt();
        msg.dest = spl.at(ARG_DEST).toInt();
        msg.data = spl.at(ARG_DATA);
        handleMessage(msg);
    }
}

void Morphology::handleMessage(ArduinoMessage message)
{
    if(message.type == HELLO) {
        Arduino narduino;
        narduino.id = message.expe;
        narduino.role = (ARD_ROLE)message.data.toInt();
        bool isNew = true;
        foreach(Arduino arduino, arduinos) {
            if(arduino.id == narduino.id) {
                isNew = false;
                arduino.role = narduino.role;
                emit(arduinoListUpdate(arduinos));
            }
        }
        if(isNew) {
            arduinos.append(narduino);
            emit(arduinoListUpdate(arduinos));
        }
    } else {
        qDebug() << "Unknown command";
    }
}

void Morphology::sendMessage(MSG_TYPE type, int id, int dest, int data)
{
    QStringList args;
    args << QString::number(type) << QString::number(id) << QString::number(dest) << QString::number(ARD_MASTER) << QString::number(data);
    m_port->write(args.join(ARG_SEP).append(MSG_SEP).toLatin1());
}
