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
#include "qextserialport.h"
#include "transaction.h"
#include "../../interfaces/interfaces.h"

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
        return ret.arg(expe, dest, QString::number(type));
    }
};


struct Arduino
{
    QString id;
    ARD_ROLE role;
    bool hasAnswered;
};

Q_DECLARE_METATYPE(Arduino)

/**
 * @brief The ArduinoCommunication class allows communication via
 * one xbee module to multible arduinos.
 */
class ArduinoCommunication : public QObject
{
    Q_OBJECT
public:
    explicit ArduinoCommunication(QObject *parent = 0);

    /**
     * @brief getMotorsNames
     * @return list of morphology motor names. (compile time determined by interface/morphology.h)
     */
    const char **getMotorsNames();

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


signals:
    /**
     * @brief arduinoAdded is emitted when a new arduino is detected
     * @param arduino
     */
    void arduinoAdded(Arduino arduino);

    /**
     * @brief arduinoRemoved is emitted when a arduino stopped responding
     * @param arduino
     */
    void arduinoRemoved(Arduino arduino);

    /**
     * @brief motorDistanceChanged is emitted when a arduino indicate a motor rotation
     * @param arduino
     * @param motor
     * @param ms
     */
    void motorDistanceChanged(QString arduino, int motor, int distance);

    /**
     * @brief angleChanged is emitted when the angle of the turntable changed
     * @param arduino
     * @param angle
     */
    void angleChanged(QString arduino, int angle);
    
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
     * @brief motorsPositionMessage sends a message to get position of all motors of the arduino
     *
     * @param arduino
     * @return the message (sendable with ->launch)
     */
    Transaction *motorsPositionMessage(QString arduino);

    /**
     * @brief completeTurnMessage sends a message to do a complete rotation
     * @param arduino
     * @return
     */
    Transaction *completeTurnMessage();
    
private slots:
    void onDataAvailable();
    void checkAliveDevices();
    void cleanUpDeadDevices();
    void deleteTransaction(int transaction);
    void _sendMessage(MSG_TYPE type, int id, QString dest, QList<QVariant> datas = QList<QVariant>());

private:
    void handleMessage(QString message);
    void handleMessage(ArduinoMessage message);
    Transaction *createTransaction(MSG_TYPE type, QString dest = DEST_BROADCAST, QList<QVariant> datas = QList<QVariant>());

    QextSerialPort *m_port;
    QString m_port_name;
    QString m_messagePart;
    QList<Arduino> m_arduinos;
    QTimer m_aliveTimer;
    QHash<int, Transaction*> m_watchers;
    //QVector<Transaction*> m_watchers;
    int m_lastMessage;
    bool m_pinging;
};

#endif // MORPHOLOGY_H
