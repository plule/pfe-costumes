#include "SlotLog.h"

void SlotLog::error(const char *error) {
	qDebug() << "SlotLog error : " << error;
}

void SlotLog::error(const char *error, const char *responsible) {
	qDebug() << "SlotLog error : " << responsible << " msg : " << error;
}

void SlotLog::idle(const char *camera) {
//	qDebug() << camera << " is idle";
}

void SlotLog::message(const char *message, const char *camera) {
	qDebug() << "Slotlog message : " << camera << " msg : " << message;
}

void SlotLog::progress_update(int id, float current, const char *camera) {
	qDebug() << "Progress : " << current << " (" << camera << ")";
}
