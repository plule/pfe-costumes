#include "messagewatcher.h"

MessageWatcher::MessageWatcher(QObject *parent) :
    QObject(parent)
{
    this->timer = new QTimer(this);
    this->type = MSG_INVALID;
    this->id = -1;
    this->dest = -1;
}

MessageWatcher::MessageWatcher(MSG_TYPE type, int id, int dest, QList<QVariant> datas, QObject *parent) :
    QObject(parent)
{
    this->type = type;
    this->id = id;
    this->dest = dest;
    this->datas = datas;

    qDebug() << "watching " << toString();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeout()));
    timer->start(1000);
}

void MessageWatcher::setAck()
{
    qDebug() << "ack " << toString();
    timer->stop();
    emit ack();
}

void MessageWatcher::setDone()
{
    qDebug() << "done " << toString();
    emit done();
}

void MessageWatcher::timeout()
{
    qDebug() << "timeout " << toString();
    emit needResend(type, id, dest, datas);
}

QString MessageWatcher::typeToString()
{
    switch(type) {
    case MSG_HELLO:
        return QString("Hello");
        break;
    case MSG_DISCOVER:
        return QString("Discover");
        break;
    case MSG_RENAME:
        return QString("Rename");
        break;
    case MSG_MORPHOLOGY:
        return QString("Morphology");
        break;
    case MSG_ROTATION:
        return QString("Rotation");
        break;
    case MSG_ACK:
        return QString("Ack");
        break;
    case MSG_DONE:
        return QString("Done");
        break;
    case MSG_SERVO_POS:
        return QString("Servo pos");
        break;
    case MSG_DEBUG:
        return QString("Debug");
        break;
    default:
        return QString("Unknown");
        break;
    }
}

QString MessageWatcher::toString()
{
    return QString::number(id) + " (" + typeToString() + ")";
}
