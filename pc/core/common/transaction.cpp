#include "transaction.h"

Transaction::Transaction(QObject *parent) :
    QObject(parent)
{
    this->m_ackTimer = new QTimer(this);
    this->m_doneTimer = new QTimer(this);
    this->m_type = MSG_INVALID;
    this->m_id = -1;
    this->m_dest = -1;
}

Transaction::Transaction(MSG_TYPE type, int id, QString dest, QList<QVariant> datas, QObject *parent) :
    QObject(parent)
{
    m_type = type;
    m_id = id;
    m_dest = dest;
    m_datas = datas;
    m_finished = false;
    m_ackTimer = 0;
    m_doneTimer = 0;
}

Transaction::~Transaction()
{
    delete m_ackTimer;
    delete m_doneTimer;
}

void Transaction::setAck()
{
    m_ackTimer->stop();
    if(m_doneTimer == 0) {
        m_finished = true;
        emit finished(m_id);
    }
    emit received();
}

void Transaction::setDone(bool success)
{
    if(m_doneTimer != 0)
        m_doneTimer->stop();
    m_finished = true;
    emit finished(m_id);
    emit done(success);
}

void Transaction::setProgress(int progressValue)
{
    emit progress(progressValue);
}

bool Transaction::valid()
{
    return m_type != MSG_INVALID;
}

void Transaction::ackTimeout()
{
    if(!m_finished)
        emit send(m_type, m_id, m_dest, m_datas);
}

void Transaction::doneTimeout()
{
    if(!m_finished)
        emit done(false);
}

void Transaction::launch()
{
    if(m_ackTimer != 0)
        m_ackTimer->start();
    if(m_doneTimer != 0)
        m_doneTimer->start();
    emit send(m_type, m_id, m_dest, m_datas);
}

void Transaction::watchForDone(int time)
{
    m_doneTimer = new QTimer(this);
    m_doneTimer->setInterval(time);
    m_doneTimer->setSingleShot(true);
    connect(m_doneTimer, SIGNAL(timeout()), this, SLOT(doneTimeout()));
}

void Transaction::watchForAck(int time)
{
    m_ackTimer = new QTimer(this);
    m_ackTimer->setInterval(time);
    m_ackTimer->setSingleShot(true);
    connect(m_ackTimer, SIGNAL(timeout()),this, SLOT(ackTimeout()));
}

void Transaction::cancel()
{
    this->m_finished = true;
    emit finished(m_id);;
}

QString Transaction::typeToString()
{
    switch(m_type) {
    case MSG_HELLO:
        return QString("Hello");
        break;
    case MSG_DISCOVER:
        return QString("Discover");
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

QString Transaction::toString()
{
    return QString::number(m_id) + " (" + typeToString() + ")";
}

QList<QVariant> Transaction::getDatas() const
{
    return m_datas;
}

QString Transaction::getDest() const
{
    return m_dest;
}

int Transaction::getId() const
{
    return m_id;
}

MSG_TYPE Transaction::getType() const
{
    return m_type;
}
