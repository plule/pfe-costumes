#ifndef _QCAMERA_H_
#define _QCAMERA_H_

#include <QObject>
#include <QString>
#include <QDebug>
#include <QFile>
#include <gphoto2/gphoto2-camera.h>

class QCamera : public QObject
{
    Q_OBJECT
public:
    QCamera();
    QCamera(const char *model, const char *port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist);
	virtual ~QCamera();

	QString getSummary();
    int capture();
	int captureToFile(const char *path);
	int captureToFile(QFile *file);
	CameraAbilities getAbilities();

protected:
	friend void error_func(GPContext *context, const char *format, va_list args, void *data);
	friend void idle_func(GPContext *context, void *data);
	friend void status_func(GPContext *context, const char *format, va_list args, void *data);
	friend void message_func(GPContext *context, const char *format, va_list args, void *data);
	friend unsigned int progress_start_func(GPContext *context, float target, const char *format, va_list args, void *data);
	friend void progress_update_func(GPContext *context, unsigned int id, float current, void *data);
	friend void progress_stop_func(GPContext *context, unsigned int id, void *data);
	
private:
    Camera *camera;
	GPContext *context;
	CameraAbilities abilities;
	GPPortInfo portinfo;
	

	int handleError(int error, QString msg);
	int buildCamera(const char *model, const char *port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist);

signals:
	void idle(const char *camera);
//	void info(const char *info, const char *camera);
	void error(const char *error, const char *camera);
	void status(const char *status, const char *camera);
	void message(const char *message, const char *camera);
	void progress_start(int id, const char *task, float target, const char *camera);
	void progress_update(int id, float current, const char *camera);
	void progress_stop(int id, const char *camera);
};

#endif
