#include "SlotLog.h"

void SlotLog::error(QString error) {
    qDebug() << "Slot error : " << sender()->objectName() << " : " << error;
}

void SlotLog::idle() {
//	qDebug() << camera << " is idle";
}

void SlotLog::message(QString message) {
    qDebug() << "Slot message : " << sender()->objectName() << " : " << message;
}

void SlotLog::progress_update(int current) {
    qDebug() << "Slot progress : " << sender()->objectName() << " : " << current;
}

void SlotLog::progress_start(QString task, int target) {
    qDebug() << "Slot start task : " << sender()->objectName() << " : " << task;
}
