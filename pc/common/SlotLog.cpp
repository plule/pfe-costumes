#include "SlotLog.h"

void SlotLog::error(QString error) {
	qDebug() << "SlotLog error : " << error;
}

void SlotLog::error(QString error, QString responsible) {
	qDebug() << "SlotLog error : " << responsible << " msg : " << error;
}

void SlotLog::idle(QString camera) {
//	qDebug() << camera << " is idle";
}

void SlotLog::message(QString message, QString camera) {
	qDebug() << "Slotlog message : " << camera << " msg : " << message;
}

void SlotLog::progress_update(int id, float current, QString camera) {
	qDebug() << "Progress : " << current << " (" << camera << ")";
}

void SlotLog::progress_start(int id, QString task, float target, QString camera) {
	qDebug() << "Start task : " << camera << " " << task;
}
