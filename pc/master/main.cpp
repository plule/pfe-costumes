#include <QtCore/QCoreApplication>
#include <QDebug>

#include "cameraHandler/cameraHandler.h"
#include "cameraHandler/QCamera.h"
 
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
	QCamera **cameras;
    qDebug() << "hello world!" << endl;
    CameraHandler::init();
    qDebug() << CameraHandler::getNbCameras() << endl;
	if(CameraHandler::getCameras(&cameras) >= 1) {
		cameras[0]->capture();
	}
	CameraHandler::close();
//    return a.exec();
}
