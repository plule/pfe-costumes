#ifndef _QCAMERA_H_
#define _QCAMERA_H_

#include <QObject>
#include <QString>
#include <QDebug>
#include <gphoto2/gphoto2-camera.h>

class QCamera : public QObject
{
    Q_OBJECT
 public:
    QCamera();
    QCamera(const char *model, const char *port, GPContext *context, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist);

	QString getSummary();
    int capture();
    
 private:
    Camera *camera;
	GPContext *context;
	CameraAbilities abilities;
	GPPortInfo portinfo;

	int handleError(int error, QString msg);
	int buildCamera(const char *model, const char *port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist);
};

#endif
