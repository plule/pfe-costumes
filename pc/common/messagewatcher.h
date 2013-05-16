#ifndef MESSAGEWATCHER_H
#define MESSAGEWATCHER_H

#include <QObject>
#include <QList>
#include <QTimer>
#include <QDebug>
#include "../../interfaces/interfaces.h"

class MessageWatcher : public QObject
{
    Q_OBJECT
public:
    explicit MessageWatcher(QObject *parent = 0);
    MessageWatcher(MSG_TYPE m_type, int m_id, int m_dest, QList<QVariant> m_datas, QObject *parent = 0);
    ~MessageWatcher();
    void setAck();
    void setDone();
    bool valid();
    
    MSG_TYPE getType() const;

    int getId() const;

    int getDest() const;

    QList<QVariant> getDatas() const;

signals:
    void ack();
    void done(bool success);
    void needResend(MSG_TYPE m_type, int m_id, int m_dest, QList<QVariant> m_datas);
    
public slots:
    void ackTimeout();
    void doneTimeout();

private:
    QString typeToString();
    QString toString();

    MSG_TYPE m_type;
    int m_id;
    int m_dest;
    QList<QVariant> m_datas;
    QTimer *m_ackTimer;
    QTimer *m_doneTimer;
};

#endif // MESSAGEWATCHER_H
