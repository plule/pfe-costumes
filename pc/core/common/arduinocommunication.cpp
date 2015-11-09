#include "arduinocommunication.h"

#define X(pin, role, define, string) string,
static const QString morpho_motors_name[] = {
    #include "morphology.h"
};
#undef X

#define X(pin, role, define, string) role,
static const MOTOR_TYPE morpho_motors_role[] = {
    #include "morphology.h"
};
#undef X

ArduinoCommunication::ArduinoCommunication(QObject *parent) :
    QObject(parent)
{
    for(int i=1; i<=MAX_ID; i++)
        m_idPool.append(i);

    m_pinging = false;
    m_lastMessage = 0;
    m_port = 0;
    m_aliveTimer.setInterval(5000);
    connect(&m_aliveTimer, SIGNAL(timeout()), this, SLOT(checkAliveDevices()));
    m_aliveTimer.start();

    m_transactionLauncher.setSingleShot(false);
    connect(&m_transactionLauncher, &QTimer::timeout, [=](){
        Transaction *t = NULL;
        m_transactionsQueueMutex.lock();
        int count = m_transactionsQueue.count();
        if(!m_transactionsQueue.empty())
            t = m_transactionsQueue.dequeue();
        m_transactionsQueueMutex.unlock();

        if(t != NULL) {
            qDebug() << "(" << count << ") Sending " << t->toString();
            _sendMessage(t->getType(), t->getId(), t->getDest(), t->getDatas());
        }
    });

    m_transactionLauncher.start(100);
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

QList<QString> ArduinoCommunication::listDevicesOfType(ARD_ROLE role)
{
    QList<QString> list;
    const int rows = m_arduinosModel.rowCount();
    for(int i = 0; i < rows; i++) {
        QModelIndex index = m_arduinosModel.index(i);
        if(index.data(ROLE_ROLE) == role)
            list.append(index.data(ID_ROLE).toString());
    }
    return list;
}

QList<QString> ArduinoCommunication::listModel()
{
    return listDevicesOfType(ROLE_MORPHOLOGY);
}

QList<QString> ArduinoCommunication::listTurntables()
{
    return listDevicesOfType(ROLE_ROTATION);
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
    m_transactionsMutex.lock();
    foreach(Transaction *transaction, m_transactions)
        if(transaction->getType() == MSG_SET_MORPHOLOGY && transaction->getDest() == arduino && transaction->getDatas().first().toInt() == motor)
            transaction->deleteLater();
    m_transactionsMutex.unlock();

    QList<QVariant> args;
    args.append(motor);
    args.append(distance);
    return createTransaction(MSG_SET_MORPHOLOGY, arduino, args);
}

Transaction *ArduinoCommunication::rotationMessage(int angle)
{
    QList<QVariant> args;
    args.append(angle);
    if(m_arduinosModel.rowCount() > 0)
        return createTransaction(MSG_SET_ANGLE, m_arduinosModel.index(0).data(ID_ROLE).toString(), args); // TODO select rotation arduino
    else
        return new Transaction(this);
}

Transaction *ArduinoCommunication::getMotorsPositionMessage(QString arduino)
{
    return createTransaction(MSG_GET_MORPHOLOGY, arduino);
}

Transaction *ArduinoCommunication::getMotorsBoundMessage(QString arduino)
{
    return createTransaction(MSG_GET_RAW_MOTOR_BOUNDS, arduino);
}

Transaction *ArduinoCommunication::startRotation()
{
    if(m_arduinosModel.rowCount() > 0)
        return createTransaction(MSG_TURN, listTurntables().first());
    else
        return new Transaction(this);
}

Transaction *ArduinoCommunication::stopRotation()
{
    if(m_arduinosModel.rowCount() > 0)
        return createTransaction(MSG_CANCEL_TURN, listTurntables().first());
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
            QString id = m_arduinosModel.index(i).data(ID_ROLE).toString();
            QString name = m_arduinosModel.index(i).data(NAME_ROLE).toString();
            m_arduinosModel.removeRow(i);
            emit arduinoLost(id, name);
        }
    }
    m_pinging = false;
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
    qDebug() << message.toString();
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
                QHash<int,QVariant> datas;
                datas.insert(NAME_ROLE,name);
                datas.insert(ID_ROLE,id);
                datas.insert(ANSWERED_ROLE,true);
                datas.insert(ROLE_ROLE, role);
                m_arduinosModel.insertWithDatas(0,datas);
                emit arduinoDetected(id,name, role);
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
        }
        break;
    case MSG_ACK:
        m_transactionsMutex.lock();
        if(m_transactions.contains(message.id) && m_transactions.value(message.id)->valid())
            m_transactions.value(message.id)->setAck();
        m_transactionsMutex.unlock();
        break;
    case MSG_DONE:
        m_transactionsMutex.lock();
        if(m_transactions.contains(message.id) && m_transactions.value(message.id)->valid())
            m_transactions.value(message.id)->setDone(true);
        m_transactionsMutex.unlock();
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
    case MSG_RAW_MOTOR_BOUNDS:
    {
        if(message.data.size() == 3) {
            emit(motorBoundsChanged(message.expe, message.data.at(0).toInt(), message.data.at(1).toInt(), message.data.at(2).toInt()));
        } else
            qWarning() << "Invalid servo bounds message.";
        break;
    }
    default:
        qDebug() << "Unknown message";
    }
}

Transaction *ArduinoCommunication::createTransaction(MSG_TYPE type, QString dest, QList<QVariant> datas)
{
    Transaction *transaction;
    if(dest == DEST_BROADCAST)
        transaction = new Transaction(type, 0, dest, datas, this);
    else
    {
        m_idPoolMutex.lock();
        int id = m_idPool.takeFirst();
        m_idPoolMutex.unlock();
        transaction = new Transaction(type, id, dest, datas, this);

        m_transactionsMutex.lock();
        m_transactions.insert(id, transaction);
        m_transactionsMutex.unlock();

        connect(transaction, &Transaction::destroyed, [=](){
            QMutexLocker lock1(&m_idPoolMutex);
            m_idPool.append(id);
            QMutexLocker lock2(&m_transactionsMutex);
            m_transactions.take(id);
            QMutexLocker lock3(&m_transactionsQueueMutex);
            m_transactionsQueue.removeAll(transaction);
        });
        transaction->watchForAck();
    }

    connect(transaction, &Transaction::send, [=](){
        QMutexLocker lock(&m_transactionsQueueMutex);
        m_transactionsQueue.enqueue(transaction);
    });

    connect(transaction, SIGNAL(finished(int)), transaction, SLOT(deleteLater()));

    return transaction;
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
