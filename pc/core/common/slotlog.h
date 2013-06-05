#ifndef _SLOTLOG_H_
#define _SLOTLOG_H_
#include <QObject>
#include <QDebug>

/**
 * @brief The SlotLog class display debug info on message sent to it
 */
class SlotLog : public QObject
{
	Q_OBJECT
public:
	SlotLog(){};
	virtual ~SlotLog(){};

public slots:
    void error(QString error);
    void idle();
    void message(QString message);
    void progress_update(int current);
    void progress_start(QString task, int target);
};


#endif