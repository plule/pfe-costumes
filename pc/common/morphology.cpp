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
    lastMessage = 0;
    m_port = new QextSerialPort(name);
    connect(m_port, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
    m_port->open(QIODevice::ReadWrite);
    aliveTimer.setInterval(5000);
    connect(&aliveTimer, SIGNAL(timeout()), this, SLOT(checkAliveDevices()));
    watchers.resize(MAX_ID);
    watchers.fill(0);
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

MessageWatcher *Morphology::sendHelloMessage()
{
    return sendMessage(MSG_DISCOVER);
}

MessageWatcher *Morphology::setMotorMicrosecond(int arduino, int motor, int ms)
{
    for(int i=0; i<MAX_ID; i++) {
        MessageWatcher *watcher = watchers[i];
        if(watcher != 0 && watcher->getDest() == arduino && watcher->getDatas().size() >= 1 && watcher->getDatas().first().toInt() == motor) {
            delete watcher;
            watchers[i] = 0;
        }
    }
    QList<QVariant> args;
    args.append(motor);
    args.append(ms);
    return sendMessage(MSG_MORPHOLOGY, arduino, args);
}

MessageWatcher *Morphology::setRotation(int angle)
{
    for(int i=0; i<MAX_ID; i++) {
        if(watchers.value(i,0) != 0 && watchers.value(i)->getType() == MSG_ROTATION) {
            delete watchers.value(i);
            watchers[i] = 0;
        }
    }

    QList<QVariant> args;
    args.append(360-angle);
    if(arduinos.size() > 0)
        return sendMessage(MSG_ROTATION, arduinos[0].id, args);
    else
        return new MessageWatcher(this);
}

MessageWatcher *Morphology::getMotorsPosition(int arduino)
{
    return sendMessage(MSG_SERVO_POS, arduino);
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
    if(message.type != MSG_ACK)
        _sendMessage(MSG_ACK, message.id, message.expe);

    switch(message.type) {
    case MSG_HELLO:
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
    case MSG_DISCOVER:
        break;
    case MSG_DEBUG:
        qDebug() << message.data;
        break;
    case MSG_RENAME:
        break;
    case MSG_MORPHOLOGY:
        break;
    case MSG_ACK:
        if(watchers.value(message.id,0) != 0 && watchers.value(message.id)->valid())
            watchers.value(message.id)->setAck();
        break;
    case MSG_DONE:
        if(watchers.value(message.id,0) != 0 && watchers.value(message.id)->valid())
            watchers.value(message.id)->setDone();
        break;
    case MSG_SERVO_POS:
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

MessageWatcher *Morphology::sendMessage(MSG_TYPE type, int dest, QList<QVariant> datas)
{
    lastMessage++;
    lastMessage = lastMessage%MAX_ID;
    if(lastMessage == DEST_BROADCAST)
        lastMessage++;
    int id = lastMessage;
    delete watchers.value(id); // old old message
    MessageWatcher *watcher = 0;
    if(dest != DEST_BROADCAST) { // not a broadcast, so we watch for answer
        watcher = new MessageWatcher(type, id, dest, datas, this);
        connect(watcher, SIGNAL(needResend(MSG_TYPE,int,int,QList<QVariant>)), this, SLOT(_sendMessage(MSG_TYPE,int,int,QList<QVariant>)));
        watchers[id] = watcher;
    }

    _sendMessage(type, id, dest, datas);
    return watcher;
}

void Morphology::_sendMessage(MSG_TYPE type, int id, int dest, QList<QVariant> datas)
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
