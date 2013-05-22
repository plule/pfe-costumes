#ifndef MESSAGEWATCHER_H
#define MESSAGEWATCHER_H

#include <QObject>
#include <QList>
#include <QTimer>
#include <QDebug>
#include "../../interfaces/interfaces.h"

class Transaction : public QObject
{
    Q_OBJECT
public:
    explicit Transaction(QObject *parent = 0);
    Transaction(MSG_TYPE m_type, int m_id, int m_dest, QList<QVariant> m_datas, QObject *parent = 0);
    ~Transaction();
    bool valid();
    void watchForDone(int time = 1000);

    void watchForAck(int time = 200);
    
    MSG_TYPE getType() const;

    int getId() const;

    int getDest() const;

    QList<QVariant> getDatas() const;

signals:
    // Message has been received by the device
    void received();
    // Work has been done by the device
    void done(bool success);
    // The message needs to be sent
    void send(MSG_TYPE m_type, int m_id, int m_dest, QList<QVariant> m_datas);
    
public slots:
    // Send the message
    void launch();
    // Cancel the message (give up retries)
    void cancel();
    // Indicate reception of the message
    void setAck();
    // Indicate if work has been done
    void setDone(bool success);

protected slots:
    void ackTimeout();
    void doneTimeout();

private:
    QString typeToString();
    QString toString();

    MSG_TYPE m_type;
    int m_id;
    int m_dest;
    bool m_finished;

    QList<QVariant> m_datas;
    QTimer *m_ackTimer;
    QTimer *m_doneTimer;
};

#endif // MESSAGEWATCHER_H
