#include "morphology.h"

Morphology::Morphology(QObject *parent) :
    QObject(parent)
{

}

Morphology::Morphology(QString name, QObject *parent) : QObject(parent)
{
    pinging = false;
    m_port = new QextSerialPort(name);
    connect(m_port, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
    m_port->open(QIODevice::ReadWrite);
    aliveTimer.setInterval(2000);
    connect(&aliveTimer, SIGNAL(timeout()), this, SLOT(checkAliveDevices()));
    aliveTimer.start();
}

void Morphology::sendHelloMessage()
{
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

void Morphology::checkAliveDevices()
{
    if(!pinging) {
        pinging = true;
        QMutableListIterator<Arduino> i(arduinos);
        while(i.hasNext())
            i.next().hasAnswered = false;
        sendHelloMessage();
    }
    QTimer::singleShot(1000, this, SLOT(cleanUpDeadDevices()));
}

void Morphology::cleanUpDeadDevices()
{
    bool changed = false;
    QMutableListIterator<Arduino> i(arduinos);
    while(i.hasNext()) {
        if(!i.next().hasAnswered)
        {
            changed = true;
            i.remove();
        }
    }
    pinging = false;
    if(changed)
        emit(arduinoListUpdate(arduinos));
}

void Morphology::handleMessage(QString message)
{
    QStringList spl = message.split(ARG_SEP);
    if(spl.size() < ARG_NUMBER)
        return;
    int dest = spl.takeFirst().toInt();
    if(dest == ARD_MASTER) {
        ArduinoMessage msg;
        msg.dest = dest;
        msg.expe = spl.takeFirst().toInt();
        msg.id = spl.takeFirst().toInt();
        msg.type = (MSG_TYPE)spl.takeFirst().toInt();
        msg.data = spl;
        handleMessage(msg);
    }
}

void Morphology::handleMessage(ArduinoMessage message)
{
    switch(message.type) {
    case HELLO:
    {
        Arduino narduino;
        narduino.id = message.expe;
        narduino.role = (ARD_ROLE)message.data.first().toInt();
        narduino.hasAnswered = true;
        bool isNew = true;
        QMutableListIterator<Arduino> i(arduinos);
        while(i.hasNext()) {
            if(i.next().id == narduino.id) {
                isNew = false;
                i.value().role = narduino.role;
                i.value().hasAnswered = true;
                emit(arduinoListUpdate(arduinos));
            }
        }
        if(isNew) {
            arduinos.append(narduino);
            emit(arduinoListUpdate(arduinos));
        }
        break;
    }
    case DISCOVER:
        break;
    case RENAME:
        break;
    case COMMAND:
        break;
    case ACK:
        break;
    case DONE:
        break;
    case DATA:
        break;
    default:
        qDebug() << "Unknown message";
    }
}

void Morphology::sendMessage(MSG_TYPE type, int id, int dest, int data)
{
    QStringList args;
    args << QString::number(type) << QString::number(id) << QString::number(dest) << QString::number(ARD_MASTER) << QString::number(data);
    m_port->write(args.join(ARG_SEP).append(MSG_SEP).toLatin1());
}
