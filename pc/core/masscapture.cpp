#include "masscapture.h"

MassCapture::MassCapture(QObject *parent) :
    QObject(parent)
{
}

void MassCapture::massCapture(QPhoto::QCamera *camera, ArduinoCommunication *morphology, Collection *collection, int idCostume, int nbPhoto)
{
    qDebug() << "mass capture";
    m_step = 360.0 / nbPhoto;
    m_actionNumber = 0;
    m_target = nbPhoto;
    setCamera(camera);
    m_morphology = morphology;
    m_collection = collection;
    m_idCostume = idCostume;
    m_problem = NoProblem;

    m_morphology->rotationMessage(0)->launch();
    connect(m_camera, SIGNAL(finished(int,QString,QStringList)), this, SLOT(launchMassCapture()));
    m_camera->captureToFile("/tmp/dummy.jpg");

    /*Transaction *watcher = m_morphology->rotationMessage(m_step*m_actionNumber);
    watcher->watchForDone();
    connect(watcher, SIGNAL(done(bool)), this, SLOT(onRotationDone(bool)));
    watcher->launch();*/
}

void MassCapture::setCamera(QPhoto::QCamera *camera)
{
    if(camera != 0) {
        connect(camera, SIGNAL(finished(int,QString,QStringList)), this, SLOT(onCaptureDone(int,QString,QStringList)));
        connect(camera, &QPhoto::QCamera::destroyed, [=]() {
            m_camera = 0;
        });
        m_camera = camera;
    }
}

void MassCapture::onCaptureDone(int status, QString path, QStringList errors)
{
    qDebug() << "capture done";
    if(status != QPhoto::QCamera::OK) {
        disconnect(m_camera, 0, this, 0);
        m_problem = CameraProblem;
        emit problem(CameraProblem);
    } else {
        emit progress(m_actionNumber, path);
        if(m_actionNumber >= m_target-1) {
            disconnect(m_camera, 0, this, 0);
            emit done();
        } else {
            m_actionNumber++;
            Transaction *message = m_morphology->rotationMessage(m_step*m_actionNumber);
            message->watchForDone();
            connect(message, SIGNAL(done(bool)), this, SLOT(onRotationDone(bool)));
            message->launch();
        }
    }
}

void MassCapture::onRotationDone(bool success)
{
    if(success) {
        if(m_camera != 0) {
            QString path = m_collection->getNewFilePath(m_idCostume, "turntable", m_settings.value("rawextension").toString());
            m_camera->captureToFile(path);
        } else {
            m_problem = CameraProblem;
            emit problem(m_problem);
        }
    } else {
        if(sender() != 0)
            disconnect(sender(), 0, this, 0);
        Transaction *watcher = m_morphology->rotationMessage(m_step*m_actionNumber);
        watcher->watchForDone();
        if(watcher->valid()) {
            connect(watcher, SIGNAL(done(bool)), this, SLOT(onRotationDone(bool)));
            watcher->launch();
        } else {
            m_problem = RotationProblem;
            emit problem(RotationProblem);
        }
    }
}

void MassCapture::onAngleChanged(int angle)
{
    qDebug() << angle;
    if(angle > m_nextAnglePhoto) {
        QString path = m_collection->getNewFilePath(m_idCostume, "turntable", m_settings.value("rawextension").toString());
        m_camera->captureToFile(path);
        // Take Photo
        m_nextAnglePhoto += m_step;
        qDebug() << "souriez";
        qDebug() << m_nextAnglePhoto;
    }
}

void MassCapture::launchMassCapture()
{
    disconnect(m_camera, 0, this, 0);
    connect(m_camera, SIGNAL(finished(int,QString,QStringList)), this, SLOT(onCaptured(int,QString,QStringList)));
    Transaction *watcher = m_morphology->completeTurnMessage();
    watcher->watchForDone(3600*1000);
    m_nextAnglePhoto = 0;
    connect(watcher, SIGNAL(progress(int)), this, SLOT(onAngleChanged(int)));
    connect(watcher, &Transaction::done, [=](bool success){
        emit done();
    });
    watcher->launch();
}

void MassCapture::onCaptured(int status, QString path, QStringList errors)
{
    emit progress((m_nextAnglePhoto-1)/m_step, path);
}

void MassCapture::resume()
{
    qDebug() << "resume";
    switch(m_problem) {
    case RotationProblem:
        m_problem = NoProblem;
        onRotationDone(false);
        break;
    case CameraProblem:
        m_problem = NoProblem;
        onRotationDone(true);
        break;
    case NoProblem:
    default:
        break; // got resume() while running
    }
}
