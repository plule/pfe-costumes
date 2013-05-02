#include "morphology.h"
#include "../../interfaces/interfaces.h"

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
    QString msg = "%1 %2 %3 %4\n";
    msg.arg(QString::number(COMMAND),"42","3","1");
    m_port->write(msg.toLatin1());
}

void Morphology::onDataAvailable()
{
    message_part.append(m_port->readAll());
    QStringList messages = message_part.split("\n");
    message_part = messages.takeLast(); // The last message is incomplete
    foreach(QString message, messages)
        qDebug() << message;
}
