#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QCamera **cameras;
    logger = new SlotLog();
    handler = &CameraHandler::Instance();
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
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QCamera **cameras;
    if(handler->getCameras(&cameras) >= 1) {
        cameras[0]->captureToFile("test.jpg");
        //		QString cameraPath;
        //		cameras[0]->captureToCamera(&cameraPath);
        //		qDebug() << cameraPath;
    }
}
