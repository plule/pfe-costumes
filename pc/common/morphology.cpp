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
    QString msg = "%1 %2 %3 %4";
    msg.arg("1","2","3","4");
    m_port->write(msg.toLatin1());
}

void Morphology::onDataAvailable()
{
    message.append(m_port->readAll());
    if(message.endsWith("\n")) {
        qDebug() << message;
        message.clear();
    }
}
