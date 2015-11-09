#ifndef MESSAGEWATCHER_H
#define MESSAGEWATCHER_H

#include <QObject>
#include <QList>
#include <QTimer>
#include <QDebug>
#include "interfaces.h"

/**
 * @brief The Transaction class represents a exchange between a arduino and a pc
 * This object is created by the Communication object only.
 * A message will be resent until ack() is received
 * You can watch for a done message to
 */
class Transaction : public QObject
{
    Q_OBJECT
public:
    explicit Transaction(QObject *parent = 0);
    Transaction(MSG_TYPE m_type, int m_id, QString m_dest, QList<QVariant> m_datas, QObject *parent = 0);
    ~Transaction();

    /**
     * @brief valid
     * @return validity
     */
    bool valid();

    /**
     * @brief watchForDone tells to wait for a done message during «time» millis
     * @param time
     */
    void watchForDone(int time = 1000);

    /**
     * @brief watchForAck tells to wait for the ack. (default is alway on for not-broadcast)
     * @param time
     */
    void watchForAck(int time = 200);
    
    MSG_TYPE getType() const;

    int getId() const;

    QString getDest() const;

    /**
     * @brief getDatas
     * @return The datas appended to the message
     */
    QList<QVariant> getDatas() const;

    QString toString();

signals:
    // Message has been received by the device
    void received();
    // Work has been done by the device
    void done(bool success);
    // The message needs to be sent
    void send(MSG_TYPE m_type, int m_id, QString m_dest, QList<QVariant> m_datas);
    // Arbitrary progress (eg. angle for complete turn)
    void progress(int value);
    // The transaction is over and the object can be destroyed
    void finished(const int id);
    
public slots:
    // Send the message
    void launch();
    // Cancel the message (give up retries)
    void cancel();
    // Indicate reception of the message
    void setAck();
    // Indicate if work has been done
    void setDone(bool success);
    // Indicate arbirtrary progress
    void setProgress(int progressValue);

protected slots:
    void ackTimeout();
    void doneTimeout();

private:
    QString typeToString();


    MSG_TYPE m_type;
    int m_id;
    QString m_dest;
    bool m_finished;

    QList<QVariant> m_datas;
    QTimer *m_ackTimer;
    QTimer *m_doneTimer;
};

#endif // MESSAGEWATCHER_H
