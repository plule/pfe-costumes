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
	static CameraHandler& Instance();
	int getNbCameras();
	int getCameras(QCamera ***cameras);
    int refreshCameraList();
	
private:
	CameraHandler& operator= (const CameraHandler&){return m_instance;}
    CameraHandler (const CameraHandler&):QObject(){}
	int autodetect(CameraList *list, GPContext *context);
	int handleError(int error, QString msg);

	static CameraHandler m_instance;
    GPPortInfoList* portinfolist;
    CameraAbilitiesList* abilities;
	QCamera* cameras[MAX_CAMERA];
	int nCameras;
	GPContext* context;

signals:
    void refreshed();
};

#endif
