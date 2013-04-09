#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QCamera **cameras;
    logger = new SlotLog();
    handler = new CameraHandler();
    ui->setupUi(this);
    displayer = new QPhotoDisplayer(ui->centralwidget);
    displayer->resize(800,600);
    displayer->setMinimumSize(300,200);
    ui->picLayout->addWidget(displayer);
    ui->centralwidget->adjustSize();

    connect(handler, SIGNAL(message(QString)), this, SLOT(updateStatusBar(QString)));
    handler->init();
    handler->getCameras(&cameras);
    doConnections();
    connect(handler, SIGNAL(refreshed()), this, SLOT(refresh()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refresh()
{
    qDebug() << handler->getNbCameras();
}

void MainWindow::startWork(QString work, int target)
{
    this->ui->workBar->setFormat(sender()->objectName() + " : " + work + " (%p%)");
    this->ui->workBar->setMaximum(target);
    this->ui->workBar->setValue(0);
}

void MainWindow::updateStatusBar(QString message)
{
    if(sender() && sender()->objectName().length() > 0)
        this->statusBar()->showMessage(sender()->objectName() + " : " + message);
    else
        this->statusBar()->showMessage(message);
}

void MainWindow::displayPicture(QString path)
{
    QPixmap pic(path);
    if(!pic.isNull())
        displayer->setPixmap(pic);
}

void MainWindow::on_captureButton_clicked()
{
    QCamera **cameras;
    QString path = QDir::temp().absoluteFilePath("test.jpg");
    if(handler->getCameras(&cameras) >= 1)
        QMetaObject::invokeMethod(cameras[0], "captureToFile", Qt::QueuedConnection, Q_ARG(QString, path));
}

void MainWindow::on_refreshButton_clicked()
{
    handler->refreshCameraList();
    doConnections();
}

void MainWindow::doConnections()
{
    QCamera **cameras;
    int nConnections = handler->getCameras(&cameras);
    for(int i=0; i < nConnections; i++) {
        connect(cameras[i], SIGNAL(error(QString)), logger, SLOT(error(QString)));
        connect(cameras[i], SIGNAL(idle()), logger, SLOT(idle()));
        connect(cameras[i], SIGNAL(status(QString)), logger, SLOT(message(QString)));
        connect(cameras[i], SIGNAL(message(QString)), logger, SLOT(message(QString)));
        connect(cameras[i], SIGNAL(progress_update(int)), logger, SLOT(progress_update(int)));
        connect(cameras[i], SIGNAL(progress_start(QString, int)), logger, SLOT(progress_start(QString, int)));

        connect(cameras[i], SIGNAL(progress_start(QString,int)), this, SLOT(startWork(QString,int)));
        connect(cameras[i], SIGNAL(progress_update(int)), this->ui->workBar, SLOT(setValue(int)));
        connect(cameras[i], SIGNAL(captured(QString)), this, SLOT(displayPicture(QString)));
    }
}
