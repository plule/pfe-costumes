#include <QtCore/QCoreApplication>
#include <QDebug>

#include "cameraHandler/cameraHandler.h"
#include "cameraHandler/QCamera.h"
 
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
	QCamera **cameras;
	CameraHandler *handler = new CameraHandler();
    qDebug() << handler->getNbCameras() << endl;
	if(handler->getCameras(&cameras) >= 1) {
		cameras[0]->capture();
	}
//    return a.exec();
}
