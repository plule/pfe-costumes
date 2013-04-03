#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <gphoto2/gphoto2-camera.h>

class CameraHandler
{
 public:
    static void init();
    static int count_cameras();

 private:
    static int autodetect(CameraList *list, GPContext *context);
    static GPPortInfoList* portinfolist;
    static CameraAbilitiesList* abilities;
};

#endif
