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
    QStringList msgArgs = QStringList() << "%1" << "%2" << "%3" << "%4";
    QString msg = msgArgs.join(ARG_SEP);
    msg.append(MSG_SEP);
    msg.arg(QString::number(COMMAND),"42","3","1");
    m_port->write(msg.toLatin1());
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
    qDebug() << message.toString();
}
