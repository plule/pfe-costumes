#include <QtCore/QCoreApplication>
#include <QDebug>

#include "cameraHandler/cameraHandler.h"
 
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "hello world!" << endl;
    CameraHandler::init();
    qDebug() << CameraHandler::count_cameras() << endl;
    return a.exec();
}
