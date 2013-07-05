#include "communication.h"

#define X(pin, role, define, string) string,
static const QString morpho_motors_name[] = {
    #include "../../interfaces/morphology.h"
};
#undef X

#define X(pin, role, define, string) role,
static const MOTOR_TYPE morpho_motors_role[] = {
    #include "../../interfaces/morphology.h"
};
#undef X

ArduinoCommunication::ArduinoCommunication(QObject *parent) :
    QObject(parent)
{
    m_pinging = false;
    m_lastMessage = 0;
    m_port = 0;
    m_aliveTimer.setInterval(5000);
    connect(&m_aliveTimer, SIGNAL(timeout()), this, SLOT(checkAliveDevices()));
    m_aliveTimer.start();
}

const QString ArduinoCommunication::getMotorName(int id)
{
    if(id >= 0 && id < MOTOR_NUMBER)
        return morpho_motors_name[id];
    return "";
}

MOTOR_TYPE ArduinoCommunication::getMotorType(int id)
{
    if(id >= 0 && id < MOTOR_NUMBER)
        return morpho_motors_role[id];
    return INVALID_MOTOR;
}

int ArduinoCommunication::getMotorsNumber()
{
    return MOTOR_NUMBER;
}

bool ArduinoCommunication::isValid()
{
    return (m_port != 0);
}

bool ArduinoCommunication::testPort(QString name)
{
    QextSerialPort port(name);
    QextSerialPort *pPort = &port;
    if(name == m_port_name && m_port != 0) {
        pPort = m_port;
    }

    if(!pPort->open(QIODevice::ReadWrite))
        return false;

    pPort->write("+++");
    this->thread()->sleep(4);
    QString ans = pPort->readLine();
    if(ans.contains("OK")) {
        pPort->write("ATCN\r");
        return true;
    }

    return false;
}

Transaction *ArduinoCommunication::setRawMotorPosition(QString arduino, int motor, int position)
{
    QList<QVariant> arguments;
    arguments.append(motor);
    arguments.append(position);
    return createTransaction(MSG_SET_RAW_MOTOR, arduino, arguments);
}

Transaction *ArduinoCommunication::setClosePosition(QString arduino, int motor, int position)
{
    QList<QVariant> arguments;
    arguments.append(motor);
    arguments.append(position);
    return createTransaction(MSG_SET_CLOSE, arduino, arguments);
}

Transaction *ArduinoCommunication::setOpenPosition(QString arduino, int motor, int position)
{
    QList<QVariant> arguments;
    arguments.append(motor);
    arguments.append(position);
    return createTransaction(MSG_SET_OPEN, arduino, arguments);
}

Transaction *ArduinoCommunication::renameMessage(QString arduino, QString name)
{
    QList<QVariant> arguments;
    arguments.append(name);
    return createTransaction(MSG_RENAME, arduino, arguments);
}

QList<QString> ArduinoCommunication::listModel()
{
    QList<QString> list;
    const int rows = m_arduinosModel.rowCount();
    for(int i = 0; i < rows; i++) {
        QModelIndex index = m_arduinosModel.index(i);
        if(index.data(ROLE_ROLE) == ROLE_MORPHOLOGY)
            list.append(index.data(ID_ROLE).toString());
    }
    return list;
}

QAbstractListModel *ArduinoCommunication::model()
{
    return &m_arduinosModel;
}

Transaction *ArduinoCommunication::helloMessage()
{
    return createTransaction(MSG_DISCOVER);
}

Transaction *ArduinoCommunication::motorDistanceMessage(QString arduino, int motor, int distance)
{
    for(int i=0; i<MAX_ID; i++) {
        Transaction *watcher = m_watchers.value(i);
        if(watcher != 0 && watcher->getDest() == arduino && watcher->getDatas().size() >= 1 && watcher->getDatas().first().toInt() == motor) {
            m_watchers.take(i)->deleteLater();
        }
    }
    QList<QVariant> args;
    args.append(motor);
    args.append(distance);
    return createTransaction(MSG_SET_MORPHOLOGY, arduino, args);
}

Transaction *ArduinoCommunication::rotationMessage(int angle)
{
    for(int i=0; i<MAX_ID; i++) {
        if(m_watchers.contains(i) && m_watchers.value(i)->getType() == MSG_SET_ANGLE) {
            deleteTransaction(i);
        }
    }

    QList<QVariant> args;
    args.append(angle);
    if(m_arduinosModel.rowCount() > 0)
        return createTransaction(MSG_SET_ANGLE, m_arduinosModel.index(0).data(ID_ROLE).toString(), args); // TODO select rotation arduino
    else
        return new Transaction(this);
}

Transaction *ArduinoCommunication::motorsPositionMessage(QString arduino)
{
    return createTransaction(MSG_GET_MORPHOLOGY, arduino);
}

Transaction *ArduinoCommunication::completeTurnMessage(int time, int angle)
{
    QList<QVariant> args;
    args.append(time);
    args.append(angle);
    if(m_arduinosModel.rowCount() > 0)
        return createTransaction(MSG_TURN, m_arduinosModel.index(0).data(ID_ROLE).toString(), args);
    else
        return new Transaction(this);
}

Transaction *ArduinoCommunication::cancelTurnMessage()
{
    if(m_arduinosModel.rowCount() > 0)
        return createTransaction(MSG_CANCEL_TURN, m_arduinosModel.index(0).data(ID_ROLE).toString());
    else
        return new Transaction(this);
}

void ArduinoCommunication::onDataAvailable()
{
    if(m_port != 0) {
        m_messagePart.append(m_port->readAll());
        QStringList messages = m_messagePart.split(MSG_SEP);
        m_messagePart = messages.takeLast(); // The last message is incomplete
        foreach(QString message, messages)
            handleMessage(message);
    }
}

void ArduinoCommunication::checkAliveDevices()
{
    if(!m_pinging) {
        m_pinging = true;
        for(int i = 0; i < m_arduinosModel.rowCount(); i++) {
            m_arduinosModel.setData(m_arduinosModel.index(i), false, ANSWERED_ROLE);
        }
        helloMessage()->launch();
    }
    QTimer::singleShot(1000, this, SLOT(cleanUpDeadDevices()));
}

void ArduinoCommunication::cleanUpDeadDevices()
{
    for(int i = 0; i < m_arduinosModel.rowCount(); i++) {
        if(!m_arduinosModel.index(i).data(ANSWERED_ROLE).toBool())
        {
            emit arduinoLost(m_arduinosModel.index(i).data(ID_ROLE).toString(),m_arduinosModel.index(i).data(NAME_ROLE).toString());
            m_arduinosModel.removeRow(i);
        }
    }
    m_pinging = false;
}

void ArduinoCommunication::deleteTransaction(int id)
{
    if(m_watchers.contains(id)) {
        //m_watchers.value(id)->deleteLater();
        m_watchers.take(id)->deleteLater();
    }
}

void ArduinoCommunication::handleMessage(QString message)
{
    QStringList spl = message.split(ARG_SEP);
    if(spl.size() < ARG_NUMBER)
        return;
    QString dest = spl.takeFirst();
    if(dest == ARD_MASTER) {
        ArduinoMessage msg;
        msg.dest = dest;
        msg.expe = spl.takeFirst();
        msg.id = spl.takeFirst().toInt();
        msg.type = (MSG_TYPE)spl.takeFirst().toInt();
        msg.data = spl;
        handleMessage(msg);
    }
}

void ArduinoCommunication::handleMessage(ArduinoMessage message)
{
    if(message.type != MSG_ACK && message.type != MSG_DEBUG)
        _sendMessage(MSG_ACK, message.id, message.expe);
    switch(message.type) {
    case MSG_HELLO:
    {
        if(message.data.size() == 2) {
            QString id = message.expe;
            ARD_ROLE role = (ARD_ROLE)message.data.takeFirst().toInt();
            QString name = message.data.takeFirst();
            bool isNew = true;
            for(int i = 0; i < m_arduinosModel.rowCount(); i++) {
                QModelIndex index = m_arduinosModel.index(i);
                if(index.data(ID_ROLE).toString() == id) {
                    isNew = false;
                    m_arduinosModel.setData(index,role,ROLE_ROLE);
                    m_arduinosModel.setData(index, name, NAME_ROLE);
                    m_arduinosModel.setData(index, true, ANSWERED_ROLE);
                }
            }
            if(isNew) {
                m_arduinosModel.insertRow(0);
                QModelIndex index = m_arduinosModel.index(0);
                m_arduinosModel.setData(index,name,Qt::DisplayRole);
                m_arduinosModel.setData(index,id,Qt::UserRole);
                m_arduinosModel.setData(index, true, ANSWERED_ROLE);
                emit arduinoDetected(id,name);
            }
        } else {
            qWarning() << "Invalid hello message from model";
        }
        break;
    }
    case MSG_DISCOVER:
        break;
    case MSG_DEBUG:
        qDebug() << message.data;
        break;
    case MSG_ANGLE:
        if(!message.data.empty()) {
            emit angleChanged(message.expe, message.data.first().toInt());
            if(m_watchers.contains(message.id) && m_watchers.value(message.id)->valid())
                m_watchers.value(message.id)->setProgress(message.data.first().toInt());
        }
        break;
    case MSG_ACK:
        if(m_watchers.contains(message.id) && m_watchers.value(message.id)->valid())
            m_watchers.value(message.id)->setAck();
        break;
    case MSG_DONE:
        if(m_watchers.contains(message.id) && m_watchers.value(message.id)->valid())
            m_watchers.value(message.id)->setDone(true);
        break;
    case MSG_MORPHOLOGY:
    {
        if(message.data.size() == 3) {
            bool calibrated = (message.data.at(2).toInt() == CALIBRATED_MOTOR);
            emit(motorDistanceChanged(message.expe, message.data.at(0).toInt(), message.data.at(1).toInt(), calibrated));
        } else
            qWarning() << "Invalid servo pos message.";
        break;
    }
    default:
        qDebug() << "Unknown message";
    }
}

Transaction *ArduinoCommunication::createTransaction(MSG_TYPE type, QString dest, QList<QVariant> datas)
{
    int id = 0;
    while(m_watchers.contains(id) && id++ <= MAX_ID);
    if(id==MAX_ID) {
        qWarning() << "Watcher pool full";
        return new Transaction(this);
    }

    Transaction *watcher = new Transaction(type, id, dest, datas, this);
    connect(watcher, SIGNAL(send(MSG_TYPE,int,QString,QList<QVariant>)), this, SLOT(_sendMessage(MSG_TYPE,int,QString,QList<QVariant>)));
    connect(watcher, SIGNAL(finished(int)), watcher, SLOT(deleteLater()));
    if(dest != DEST_BROADCAST) { // not a broadcast, so we watch for answer
        connect(watcher, SIGNAL(finished(int)), this, SLOT(deleteTransaction(int)));
        m_watchers.insert(id, watcher);
        watcher->watchForAck();
    }
    return watcher;
}

void ArduinoCommunication::_sendMessage(MSG_TYPE type, int id, QString dest, QList<QVariant> datas)
{
    if(m_port != 0) {
        QStringList args;
        args
                << dest
                << ARD_MASTER
                << QString::number(id)
                << QString::number(type);
        foreach(QVariant data, datas) {
            args << data.toString();
        };
        QString message = args.join(ARG_SEP).append(MSG_SEP);
        m_port->write(message.toLatin1());
    }
}

void ArduinoCommunication::setPort(QString port)
{
    m_port_name = QString(port);
    delete m_port;
    m_port = new QextSerialPort(port);
    connect(m_port, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
    if(!m_port->open(QIODevice::ReadWrite)) {
        delete m_port;
        m_port = 0;
    } else {
        helloMessage()->launch();
    }
}
