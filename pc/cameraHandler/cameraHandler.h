#ifndef _CAMERA_H_
#define _CAMERA_H_

#define MAX_CAMERA 10

#include <gphoto2/gphoto2-camera.h>

#include "QCamera.h"

class CameraHandler: public QObject
{
	Q_OBJECT
public:
	CameraHandler();
	~CameraHandler();
	int getNbCameras();
	int getCameras(QCamera ***cameras);
    int refreshCameraList();
	void error(const char *error);
	
private:
	int autodetect(CameraList *list, GPContext *context);
	int handleError(int error, QString msg);
	
    GPPortInfoList* portinfolist;
    CameraAbilitiesList* abilities;
	QCamera* cameras[MAX_CAMERA];
	int nCameras;
	GPContext* context;

signals:
	void error_s(const char *error);
};

#endif
