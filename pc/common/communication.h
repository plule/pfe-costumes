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
#include "qextserialport.h"
#include "transaction.h"
#include "../../interfaces/interfaces.h"

struct ArduinoMessage
{
    int dest;
    int expe;
    int id;
    MSG_TYPE type;
    QStringList data;

    QString toString() {
        QString ret = "from %1 to %2 : (%3)";
        return ret.arg(QString::number(expe), QString::number(dest), QString::number(type));
    }
};


struct Arduino
{
    ard_id_t id;
    ARD_ROLE role;
    bool hasAnswered;
};

Q_DECLARE_METATYPE(Arduino)

class ArduinoCommunication : public QObject
{
    Q_OBJECT
public:
    explicit ArduinoCommunication(QObject *parent = 0);
    const char **getMotorsNames();
    int getMotorsNumber();
    bool isValid();
    bool testPort(QString name);


signals:
    void arduinoAdded(Arduino arduino);
    void arduinoRemoved(Arduino arduino);
    void motorMicrosecondChanged(int arduino, int motor, int ms);
    
public slots:
    void setPort(QString port);
    Transaction *helloMessage();
    Transaction *motorMicrosecondMessage(int arduino, int motor, int ms);
    Transaction *rotationMessage(int angle);
    Transaction *motorsPositionMessage(int arduino);
    
private slots:
    void onDataAvailable();
    void checkAliveDevices();
    void cleanUpDeadDevices();
    void _sendMessage(MSG_TYPE type, int id, int dest, QList<QVariant> datas = QList<QVariant>());

private:
    void handleMessage(QString message);
    void handleMessage(ArduinoMessage message);
    Transaction *createTransaction(MSG_TYPE type, int dest = DEST_BROADCAST, QList<QVariant> datas = QList<QVariant>());

    QextSerialPort *m_port;
    QString m_port_name;
    QString m_messagePart;
    QList<Arduino> m_arduinos;
    QTimer m_aliveTimer;
    QVector<Transaction*> m_watchers;
    int m_lastMessage;
    bool m_pinging;
};

#endif // MORPHOLOGY_H
