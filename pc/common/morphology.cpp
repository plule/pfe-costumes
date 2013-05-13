#include "morphology.h"

#define X(pin, define, string) string,
static const char *morpho_motors_name[] = {
    #include "../../interfaces/morphology.h"
};
#undef X

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
    aliveTimer.setInterval(5000);
    connect(&aliveTimer, SIGNAL(timeout()), this, SLOT(checkAliveDevices()));
    aliveTimer.start();
}

const char **Morphology::getMotorsNames()
{
    return morpho_motors_name;
}

int Morphology::getMotorsNumber()
{
    return MOTOR_NUMBER;
}

void Morphology::sendHelloMessage()
{
    sendMessage(DISCOVER, 40, 41);
}

void Morphology::setMotorMicrosecond(int arduino, int motor, int ms)
{
    QList<QVariant> args;
    args.append(motor);
    args.append(ms);
    sendMessage(COMMAND, 3, arduino, args);
}

void Morphology::setRotation(int angle)
{
    QList<QVariant> args;
    args.append(360-angle);
    sendMessage(MSG_ROTATION, 4, arduinos[0].id, args);
}

void Morphology::getMotorsPosition(int arduino)
{
    sendMessage(SERVO_POS, 1, arduino);
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
    QMutableListIterator<Arduino> i(arduinos);
    while(i.hasNext()) {
        if(!i.next().hasAnswered)
        {
            emit(arduinoRemoved(i.value()));
            i.remove();
        }
    }
    pinging = false;
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
            }
        }
        if(isNew) {
            arduinos.append(narduino);
            emit(arduinoAdded(narduino));
        }
        break;
    }
    case DISCOVER:
        break;
    case DEBUG:
        qDebug() << message.data;
        break;
    case RENAME:
        break;
    case COMMAND:
        break;
    case ACK:
        break;
    case DONE:
        break;
    case SERVO_POS:
    {
        if(message.data.size() == 2)
            emit(motorMicrosecondChanged(message.expe, message.data.at(0).toInt(), message.data.at(1).toInt()));
        else
            qWarning() << "Invalid servo pos message.";
        break;
    }
    default:
        qDebug() << "Unknown message";
    }
}

void Morphology::sendMessage(MSG_TYPE type, int id, int dest, QList<QVariant> datas)
{
    QStringList args;
    args
            << QString::number(dest)
            << QString::number(ARD_MASTER)
            << QString::number(id)
            << QString::number(type);
    foreach(QVariant data, datas) {
        args << data.toString();
    };

    m_port->write(args.join(ARG_SEP).append(MSG_SEP).toLatin1());
}
