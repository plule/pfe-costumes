#include "messagewatcher.h"

MessageWatcher::MessageWatcher(QObject *parent) :
    QObject(parent)
{
    this->ackTimer = new QTimer(this);
    this->doneTimer = new QTimer(this);
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
    ackTimer = new QTimer(this);
    ackTimer->setSingleShot(true);
    connect(ackTimer, SIGNAL(timeout()),this, SLOT(ackTimeout()));
    ackTimer->start(1000);

    if(type == MSG_ROTATION) {
        doneTimer = new QTimer(this);
        doneTimer->setSingleShot(true);
        connect(doneTimer, SIGNAL(timeout()), this, SLOT(doneTimeout()));
        doneTimer->start(5000);
    }
}

MessageWatcher::~MessageWatcher()
{
    qDebug() << "deleting " << toString();
    delete ackTimer;
    delete doneTimer;
}

void MessageWatcher::setAck()
{
    qDebug() << "ack " << toString();
    ackTimer->stop();
    emit ack();
}

void MessageWatcher::setDone()
{
    qDebug() << "done " << toString();
    doneTimer->stop();
    emit done(true);
}

bool MessageWatcher::valid()
{
    return type != MSG_INVALID;
}

void MessageWatcher::ackTimeout()
{
    qDebug() << "timeout " << toString();
    emit needResend(type, id, dest, datas);
}

void MessageWatcher::doneTimeout()
{
    qDebug() << "timeout done " << toString();
    emit done(false);
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

QList<QVariant> MessageWatcher::getDatas() const
{
    return datas;
}

int MessageWatcher::getDest() const
{
    return dest;
}

int MessageWatcher::getId() const
{
    return id;
}

MSG_TYPE MessageWatcher::getType() const
{
    return type;
}
