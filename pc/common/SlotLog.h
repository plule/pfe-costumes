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
	void error(const char *error);
	void error(const char *error, const char *responsible);
	void idle(const char *camera);
	void message(const char *message, const char *camera);
	void progress_update(int id, float current, const char *camera);
	void progress_start(int id, const char *task, float target, const char *camera);
};


#endif
