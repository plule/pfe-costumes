#ifndef _QCAMERA_H_
#define _QCAMERA_H_

#include <QObject>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QThread>
#include <QTimer>
#include <gphoto2/gphoto2-camera.h>

#define TIMEOUT 10000

class QCamera : public QObject
{
    Q_OBJECT
public:
    QCamera();
    QCamera(const char *model, const char *port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist);
	virtual ~QCamera();

	QString getSummary();
	QString getAbout();
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

public slots:
    void captureToCamera(QString *camerapath);
    void captureToFile(const char *path);
    void captureToFile(QFile *file);
    void captureToFile(QString path);

private slots:
	void timeout();

signals:
    void idle();
//	void info(const char *info, const char *camera);
    void error(QString error);
    void status(QString status);
    void message(QString message);
    void progress_start(QString task, int target);
    void progress_update(int current);
    void progress_stop();
    void captured(QString path);
};

#endif
