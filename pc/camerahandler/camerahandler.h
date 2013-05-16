#ifndef _CAMERA_H_
#define _CAMERA_H_

#define MAX_CAMERA 10
#include <QObject>
#include <QDebug>
#include <QList>
#include <QThread>

#include <gphoto2/gphoto2-camera.h>

#include "cameraexception.h"
#include "qcamera.h"

namespace QPhoto
{

/**
 * @brief The CameraHandler class allow handling of a set of QCamera object (detecting cameras, building them,...)
 */
class CameraHandler: public QObject
{
	Q_OBJECT
public:
    /**
     * @brief CameraHandler constructor. Do not connect or detect anything. init() must be called
     */
	CameraHandler();
	~CameraHandler();

    /**
     * @brief getNbCameras
     * @return number of detected cameras
     */
	int getNbCameras();

    /**
     * @brief getCameras
     * @param cameras list to be filled
     * @return number of detected cameras
     */
    int getCameras(QCamera ***m_cameras);

    /**
     * @brief refreshCameraList relaunch the autodectection process
     * @return number of cameras
     */
    int refreshCameraList();

    /**
     * @brief init autodetect the cameras
     * @return error code ? todo
     */
    int init();

    /**
     * @brief deinit frees the camera, disconnecting them
     * @return error code ? todo
     */
    int deinit();

	
private:
    int autodetect(CameraList *list, GPContext *m_context);
	int handleError(int error, QString msg);

    GPPortInfoList* m_portinfolist;
    CameraAbilitiesList* m_abilities;
    QCamera* m_cameras[MAX_CAMERA];
    int m_nCameras;
    GPContext* m_context;

signals:
    void refreshed();
    void message(QString message);
};
}

#endif
