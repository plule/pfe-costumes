#include "masscapture.h"

MassCapture::MassCapture(QObject *parent) :
    QObject(parent)
{
}

MassCapture::~MassCapture()
{
    m_morphology->cancelTurnMessage()->launch();
}

void MassCapture::massCapture(QPhoto::QCamera *camera, ArduinoCommunication *morphology, Collection *collection, int idCostume, int nbPhoto)
{
    m_step = 360.0 / nbPhoto;
    m_actionNumber = 0;
    m_target = nbPhoto;
    setCamera(camera);
    m_morphology = morphology;
    m_collection = collection;
    m_idCostume = idCostume;
    m_problem = NoProblem;
    m_rotationTime = 60;

    m_morphology->rotationMessage(0)->launch();
    launchMassCapture();
    //connect(m_camera, SIGNAL(finished(int,QString,QStringList)), this, SLOT(launchMassCapture()));
    //m_camera->captureToFile("/tmp/dummy.jpg");
}

void MassCapture::setCamera(QPhoto::QCamera *camera)
{
    if(camera != 0) {
        connect(camera, SIGNAL(finished(int,QString,QStringList)), this, SLOT(onCaptured(int,QString,QStringList)));
        connect(camera, &QPhoto::QCamera::destroyed, [=]() {
            m_camera = 0;
        });
        m_camera = camera;
    }
}

void MassCapture::onAngleChanged(int angle)
{
    m_currentAngle = angle;
    if(angle > m_nextAnglePhoto) {
        m_nextAnglePhoto += m_step;

        if(m_camera && m_camera->isConnected()) {
            QString path = m_collection->getNewFilePath(m_idCostume, "turntable", m_settings.value("rawextension").toString());
            m_camera->captureToFile(path);
        } else {
            emit problem(CameraProblem);
            return;
        }
    }
}

void MassCapture::launchMassCapture()
{
    disconnect(m_camera, 0, this, 0);
    connect(m_camera, SIGNAL(finished(int,QString,QStringList)), this, SLOT(onCaptured(int,QString,QStringList)));
    Transaction *watcher = m_morphology->completeTurnMessage(m_rotationTime);
    watcher->watchForDone(3600*1000); // One hour delay to make the complete turn
    m_nextAnglePhoto = 0;
    connect(watcher, SIGNAL(progress(int)), this, SLOT(onAngleChanged(int)));
    connect(watcher, &Transaction::done, [=](bool success){
        if(m_problem == NoProblem)
            emit done();
    });
    qDebug() << "launch";
    watcher->launch();
}

void MassCapture::onCaptured(int status, QString path, QStringList errors)
{
    if(status != QPhoto::QCamera::OK) {
        m_problem = CameraProblem;
        m_morphology->cancelTurnMessage()->launch();

        emit problem(CameraProblem);
    } else {
        emit progress((m_nextAnglePhoto-1)/m_step, path);
    }
}

void MassCapture::resume()
{
    m_problem = NoProblem;
    Transaction *watcher = m_morphology->completeTurnMessage(m_rotationTime, m_currentAngle);
    connect(watcher, &Transaction::done, [=](bool success){
        if(m_problem == NoProblem)
            emit done();
    });
    watcher->launch();
}
