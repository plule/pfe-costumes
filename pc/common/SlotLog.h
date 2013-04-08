#ifndef _SLOTLOG_H_
#define _SLOTLOG_H_
#include <QObject>
#include <QDebug>

class SlotLog : public QObject
{
	Q_OBJECT
public:
	SlotLog(){};
	virtual ~SlotLog(){};

public slots:
    void error(QString error);
    void error(QString error, QString responsible);
    void idle(QString camera);
    void message(QString message, QString camera);
    void progress_update(int id, float current, QString camera);
    void progress_start(int id, QString task, float target, QString camera);
};


#endif
