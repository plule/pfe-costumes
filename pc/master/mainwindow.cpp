#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QPhoto::QCamera **cameras;
    logger = new SlotLog();
    handler = new QPhoto::CameraHandler();
    ui->setupUi(this);
    displayer = new QTurntable(ui->centralwidget);
    displayer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    displayer->resize(800,600);
    displayer->setMinimumSize(300,200);
    displayer->setCustomController(ui->dial);
    ui->picLayout->addWidget(displayer);
    ui->centralwidget->adjustSize();

    connect(handler, SIGNAL(message(QString)), this, SLOT(updateStatusBar(QString)));
    handler->init();
    handler->getCameras(&cameras);
    doConnections();
    connect(handler, SIGNAL(refreshed()), this, SLOT(refresh()));
    displayer->setNumber(36);
    for(int i=1; i<=36; ++i)
    {
        displayer->setPixmap(i-1, QString("/home/xubuntu/PFE/monkeys/%1.png").arg(i));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete handler;
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
        displayer->addPixmap(pic);
}

void MainWindow::timeout()
{
    statusBar()->showMessage("Lost camera.");
}

void MainWindow::on_captureButton_clicked()
{
    QPhoto::QCamera **cameras;
    if(handler->getCameras(&cameras) >= 1)
    {
        QPhoto::QCamera *camera = cameras[0];
        QString path = QDir::temp().absoluteFilePath("test.jpg");
        QTimer *timer = new QTimer(this);
        timer->setSingleShot(true);
        timer->setInterval(10000);
        connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
        connect(camera, SIGNAL(captured(QString)), timer, SLOT(stop()));
        timer->start();
        QMetaObject::invokeMethod(camera, "captureToFile", Qt::QueuedConnection, Q_ARG(QString, path));
    }
}

void MainWindow::on_refreshButton_clicked()
{
    handler->refreshCameraList();
    doConnections();
}

void MainWindow::doConnections()
{
    QPhoto::QCamera **cameras;
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
        //connect(cameras[i], SIGNAL(captured(QString)), this, SLOT(displayPicture(QString)));
        connect(cameras[i], SIGNAL(captured(QString)), this->displayer, SLOT(setCurrentPixmap(QString)));

        connect(cameras[i], SIGNAL(error(QString)), this->statusBar(), SLOT(showMessage(QString)));
    }
}
