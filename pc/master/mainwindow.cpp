#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QCamera **cameras;
    logger = new SlotLog();
    handler = new CameraHandler();//&CameraHandler::Instance();

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

void MainWindow::refresh()
{
    qDebug() << handler->getNbCameras();
}

void MainWindow::on_captureButton_clicked()
{
    QCamera **cameras;
    if(handler->getCameras(&cameras) >= 1)
        qDebug() << QMetaObject::invokeMethod(cameras[0], "captureToFile", Qt::QueuedConnection, Q_ARG(QString, "test.jpg"));
        //cameras[0]->captureToFile("test.jpg");
}

void MainWindow::on_refreshButton_clicked()
{
    QCamera **cameras;
    handler->refreshCameraList();
    for(int i=0; i < handler->getCameras(&cameras); i++) {
        connect(cameras[i], SIGNAL(error(QString, QString)), logger, SLOT(error(QString, QString)));
        connect(cameras[i], SIGNAL(idle(QString)), logger, SLOT(idle(QString)));
        connect(cameras[i], SIGNAL(status(QString,QString)), logger, SLOT(message(QString,QString)));
        connect(cameras[i], SIGNAL(message(QString,QString)), logger, SLOT(message(QString,QString)));
        connect(cameras[i], SIGNAL(progress_update(int,float,QString)), logger, SLOT(progress_update(int,float,QString)));
        connect(cameras[i], SIGNAL(progress_start(int,QString, float, QString)), logger, SLOT(progress_start(int,QString, float, QString)));
    }
}
