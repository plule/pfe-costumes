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
    nCameras = handler->getCameras(&cameras);
    for(int i=0; i < nCameras; i++) {
        connect(cameras[i], SIGNAL(error(QString, QString)), logger, SLOT(error(QString, QString)));
        connect(cameras[i], SIGNAL(idle(QString)), logger, SLOT(idle(QString)));
        connect(cameras[i], SIGNAL(status(QString,QString)), logger, SLOT(message(QString,QString)));
        connect(cameras[i], SIGNAL(message(QString,QString)), logger, SLOT(message(QString,QString)));
        connect(cameras[i], SIGNAL(progress_update(int,float,QString)), logger, SLOT(progress_update(int,float,QString)));
        connect(cameras[i], SIGNAL(progress_start(int,QString, float, QString)), logger, SLOT(progress_start(int,QString, float, QString)));
    }
    connect(handler, SIGNAL(refreshed()), this, SLOT(refresh()));
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

void MainWindow::on_pushButton_2_clicked()
{
    handler->refreshCameraList();
}

void MainWindow::refresh()
{
    qDebug() << handler->getNbCameras();
}
