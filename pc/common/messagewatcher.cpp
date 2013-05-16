#include "messagewatcher.h"

MessageWatcher::MessageWatcher(QObject *parent) :
    QObject(parent)
{
    this->m_ackTimer = new QTimer(this);
    this->m_doneTimer = new QTimer(this);
    this->m_type = MSG_INVALID;
    this->m_id = -1;
    this->m_dest = -1;
}

MessageWatcher::MessageWatcher(MSG_TYPE type, int id, int dest, QList<QVariant> datas, QObject *parent) :
    QObject(parent)
{
    this->m_type = type;
    this->m_id = id;
    this->m_dest = dest;
    this->m_datas = datas;

    qDebug() << "watching " << toString();
    m_ackTimer = new QTimer(this);
    m_ackTimer->setSingleShot(true);
    connect(m_ackTimer, SIGNAL(timeout()),this, SLOT(ackTimeout()));
    m_ackTimer->start(1000);

    if(type == MSG_ROTATION) {
        m_doneTimer = new QTimer(this);
        m_doneTimer->setSingleShot(true);
        connect(m_doneTimer, SIGNAL(timeout()), this, SLOT(doneTimeout()));
        m_doneTimer->start(5000);
    }
}

MessageWatcher::~MessageWatcher()
{
    qDebug() << "deleting " << toString();
    delete m_ackTimer;
    delete m_doneTimer;
}

void MessageWatcher::setAck()
{
    qDebug() << "ack " << toString();
    m_ackTimer->stop();
    emit ack();
}

void MessageWatcher::setDone()
{
    qDebug() << "done " << toString();
    m_doneTimer->stop();
    emit done(true);
}

bool MessageWatcher::valid()
{
    return m_type != MSG_INVALID;
}

void MessageWatcher::ackTimeout()
{
    qDebug() << "timeout " << toString();
    emit needResend(m_type, m_id, m_dest, m_datas);
}

void MessageWatcher::doneTimeout()
{
    qDebug() << "timeout done " << toString();
    emit done(false);
}

QString MessageWatcher::typeToString()
{
    switch(m_type) {
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
    return QString::number(m_id) + " (" + typeToString() + ")";
}

QList<QVariant> MessageWatcher::getDatas() const
{
    return m_datas;
}

int MessageWatcher::getDest() const
{
    return m_dest;
}

int MessageWatcher::getId() const
{
    return m_id;
}

MSG_TYPE MessageWatcher::getType() const
{
    return m_type;
}
