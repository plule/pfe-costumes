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
    MessageWatcher(MSG_TYPE type, int id, int dest, QList<QVariant> datas, QObject *parent = 0);
    void setAck();
    void setDone();
    
signals:
    void ack();
    void done();
    void needResend(MSG_TYPE type, int id, int dest, QList<QVariant> datas);
    
public slots:
    void timeout();

private:
    QString typeToString();
    QString toString();

    MSG_TYPE type;
    int id;
    int dest;
    QList<QVariant> datas;
    QTimer *timer;
};

#endif // MESSAGEWATCHER_H
