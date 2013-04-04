#ifndef _CAMERA_H_
#define _CAMERA_H_

#define MAX_CAMERA 10

#include <gphoto2/gphoto2-camera.h>

#include "QCamera.h"

class CameraHandler
{
public:
	static int init();
    static int count_cameras();
    static int refreshCameraList();

private:
	static int autodetect(CameraList *list, GPContext *context);
	static int handleError(int error, QString msg);

    static GPPortInfoList* portinfolist;
    static CameraAbilitiesList* abilities;
    static bool initialized;
	static QCamera* cameras[MAX_CAMERA];
};

#endif
