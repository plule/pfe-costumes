#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QList>
#include <QMetaType>
#include <QTimer>
#include <QMutableListIterator>
#include <QStandardItemModel>
#include <QMap>
#include <QThread>
#include <QString>
#include <QPersistentModelIndex>
#include <QStack>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>
#include "qlistmodel.h"
#include "qextserialport.h"
#include "transaction.h"
#include "interfaces.h"

/**
 * @brief The ArduinoMessage struct describe a message sendable to a device
 */
struct ArduinoMessage
{
    QString dest;
    QString expe;
    int id;
    MSG_TYPE type;
    QStringList data;

    QString toString() {
        QString ret = "from %1 to %2 : (%3)";
        QString type_str;
        switch(type) {
        case MSG_ACK:
            type_str = "ack";
            break;
        case MSG_DONE:
            type_str = "done";
            break;
        case MSG_DISCOVER:
            type_str = "discover";
            break;
        case MSG_HELLO:
            type_str = "hello";
            break;
        case MSG_SET_MORPHOLOGY:
            type_str = "set morphology";
            break;
        case MSG_GET_MORPHOLOGY:
            type_str = "get morphology";
            break;
        case MSG_MORPHOLOGY:
            type_str = "morphology";
            break;
        case MSG_SET_ANGLE:
            type_str = "set angle";
            break;
        case MSG_TURN:
            type_str = "turn";
            break;
        case MSG_CANCEL_TURN:
            type_str = "cancel turn";
            break;
        case MSG_ANGLE:
            type_str = "angle";
            break;
        case MSG_SET_RAW_MOTOR:
            type_str = "set raw motor";
            break;
        case MSG_GET_RAW_MOTOR_BOUNDS:
            type_str = "get raw motor bounds";
            break;
        case MSG_RAW_MOTOR_BOUNDS:
            type_str = "raw motor bounds";
            break;
        case MSG_SET_CLOSE:
            type_str = "close";
            break;
        case MSG_SET_OPEN:
            type_str = "open";
            break;
        case MSG_RENAME:
            type_str = "rename";
            break;
        case MSG_DEBUG:
            type_str = "debug";
            break;
        case MSG_INVALID:
            type_str = "invalid";
            break;
        }

        return ret.arg(expe, dest, type_str);
    }
};

#define NAME_ROLE Qt::DisplayRole /* QString */
#define ID_ROLE Qt::UserRole /* QString */
#define ROLE_ROLE Qt::UserRole+1 /* int (ARD_ROLE) */
#define ANSWERED_ROLE Qt::UserRole+2 /* bool */

/**
 * @brief The ArduinoCommunication class allows communication via
 * one xbee module to multible arduinos.
 */
class ArduinoCommunication : public QObject
{
    Q_OBJECT
public:
    explicit ArduinoCommunication(QObject *parent = 0);

    ~ArduinoCommunication();

    /**
     * @brief getMotorsName
     * @return Morphology motor names. (compile time determined by interface/morphology.h)
     */
    const QString getMotorName(int id);

    /**
     * @brief getMotorType
     * @param id
     * @return the type of a motor (side or front), (compile time determined by interface/morphology.h)
     */
    MOTOR_TYPE getMotorType(int id);

    /**
     * @brief getMotorGroup
     * @param id
     * @return the group of a motor (bust, waist, hips, size)
     */
    MOTOR_GROUP getMotorGroup(int id);

    /**
     * @brief getMotorsNumber
     * @return Number of morphology motor. (compile time determined by interface/morphology.h
     */
    int getMotorsNumber();

    /**
     * @brief isValid
     * @return true if the port is opened
     */
    bool isValid();

    /**
     * @brief testPort determines if the chosen port seems to be a xbee
     * @param name
     * @return
     */
    bool testPort(QString name);

    /**
     * @brief setRawMotorPosition put one motor given a position (in us)
     * @param arduino
     * @param motor
     * @param position
     * @return
     */
    Transaction *setRawMotorPosition(QString arduino, int motor, int position);

    /**
     * @brief setClosePosition indicate to the arduino the "closed" position of a motor
     * @param arduino
     * @param motor
     * @return
     */
    Transaction *setClosePosition(QString arduino, int motor, int position);

    /**
     * @brief setOpenPosition indicate to the arduino the "opened" position of a motor
     * @param arduino
     * @param motor
     * @return
     */
    Transaction *setOpenPosition(QString arduino, int motor, int position);

    /**
     * @brief renameMessage set the name of a arduino
     * @param arduino
     * @param name
     * @return
     */
    Transaction *renameMessage(QString arduino, QString name);

    /**
     * @brief listModel
     * @return the list of all the known mannequins' id
     */
    QList<QString> listModel();

    /**
     * @brief listRotationControl list all the known turntables
     * @return
     */
    QList<QString> listTurntables();

    /**
     * @brief model
     * @return the model (qt model) listing all the ids. In Qt::DisplayRole is stored displayable value
     * In Qt::UserRole is stored the ids
     */
    QAbstractListModel *model();

    void deregisterTransaction(Transaction *transaction);


signals:
    /**
     * @brief motorDistanceChanged is emitted when a arduino indicate a motor rotation
     * @param arduino
     * @param motor
     * @param ms
     */
    void motorDistanceChanged(QString arduino, int motor, int distance, bool calibrated);

    /**
     * @brief motorBoundsChanged is emitted when an arduino indicate bounds of a motor
     * @param arduino
     * @param motor
     * @param umin
     * @param umax
     */
    void motorBoundsChanged(QString arduino, int motor, int umin, int umax);

    /**
     * @brief angleChanged is emitted when the angle of the turntable changed
     * @param arduino
     * @param angle
     */
    void angleChanged(QString arduino, int angle);

    void arduinoDetected(QString arduino, QString name, ARD_ROLE role);
    void arduinoLost(QString arduino, QString name);
    
public slots:
    /**
     * @brief setPort binds the object to a serial port. This should be the physical port
     * (discovered by qExtSerialEnumerator physName)
     * @param port
     */
    void setPort(QString port);

    /**
     * @brief helloMessage creates a discovering broadcast message
     * @return the message (sendable with ->launch)
     */
    Transaction *helloMessage();

    /**
     * @brief motorDistanceMessage creates a message to rotate a motor
     * @param arduino
     * @param motor
     * @param distance
     * @return the message (sendable with ->launch)
     */
    Transaction *motorDistanceMessage(QString arduino, int motor, int distance);

    /**
     * @brief rotationMessage creates a message to do the global rotation
     * @param angle
     * @return the message (sendable with ->launch)
     */
    Transaction *rotationMessage(int angle);

    /**
     * @brief motorsPositionMessage creates a message to get position of all motors of the arduino
     *
     * @param arduino
     * @return the message (sendable with ->launch)
     */
    Transaction *getMotorsPositionMessage(QString arduino);

    /**
     * @brief motorsBoundMessage creates a message to get the bound of all motors of the arduino
     * @param arduino
     * @return the message (sendable with ->launch)
     */
    Transaction *getMotorsBoundMessage(QString arduino);

    /**
     * @brief completeTurnMessage creates a message to do a complete rotation.
     * @param time is the total time required for the operation
     * @returnthe message (sendable with ->launch)
     */
    Transaction *startRotation();

    /**
     * @brief cancelTurnMessage sends a message to stop a rotation
     * @return
     */
    Transaction *stopRotation();
    
private slots:
    void onDataAvailable();
    void checkAliveDevices();
    void cleanUpDeadDevices();
    QList<QString> listDevicesOfType(ARD_ROLE role);
    void _sendMessage(MSG_TYPE type, int id, QString dest, QList<QVariant> datas = QList<QVariant>());

private:
    void handleMessage(QString message);
    void handleMessage(ArduinoMessage message);
    Transaction *createTransaction(MSG_TYPE type, QString dest = DEST_BROADCAST, QList<QVariant> datas = QList<QVariant>());

    QextSerialPort *m_port;
    QString m_port_name;
    QString m_messagePart;
    //QList<Arduino> m_arduinos;
    QListModel m_arduinosModel;
    QTimer m_aliveTimer;
    QTimer m_transactionLauncher;
    QHash<int,Transaction*> m_transactions;
    QQueue<Transaction*> m_transactionsQueue;
    QStack<int> m_idPool;
    QMutex m_transactionsQueueMutex;
    QMutex m_idPoolMutex;
    QMutex m_transactionsMutex;
    int m_lastMessage;
    bool m_pinging;
};

#endif // MORPHOLOGY_H
