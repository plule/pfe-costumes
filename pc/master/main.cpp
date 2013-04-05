#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QObject>

#include "cameraHandler/cameraHandler.h"
#include "cameraHandler/QCamera.h"
#include "common/SlotLog.h"
 
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
	QCamera **cameras;
	SlotLog *logger = new SlotLog();
	CameraHandler *handler = &CameraHandler::Instance();
	int nCameras;
    qDebug() << handler->getNbCameras() << endl;
	nCameras = handler->getCameras(&cameras);
	for(int i=0; i < nCameras; i++) {
		QObject::connect(cameras[i], SIGNAL(error(const char*, const char*)), logger, SLOT(error(const char*, const char*)));
		QObject::connect(cameras[i], SIGNAL(idle(const char*)), logger, SLOT(idle(const char*)));
		QObject::connect(cameras[i], SIGNAL(status(const char*,const char*)), logger, SLOT(message(const char*,const char*)));
		QObject::connect(cameras[i], SIGNAL(message(const char*,const char*)), logger, SLOT(message(const char*,const char*)));
		QObject::connect(cameras[i], SIGNAL(progress_update(int,float,const char*)), logger, SLOT(progress_update(int,float,const char*)));
		QObject::connect(cameras[i], SIGNAL(progress_start(int,const char*, float, const char*)), logger, SLOT(progress_start(int,const char*, float, const char*)));
	}
		
	if(handler->getCameras(&cameras) >= 1) {
		cameras[0]->captureToFile("test.jpg");
	}
//    return a.exec();
}
