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
        connect(camera, &QPhoto::QCamera::finished, this, &MassCapture::onCaptured);
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
            emit problem(CameraProblem, tr("Camera seems to be disconnected"));
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
    connect(watcher, &Transaction::progress, this, &MassCapture::onAngleChanged);
    connect(watcher, &Transaction::done, this, &MassCapture::onRotationDone);
    watcher->launch();
}

void MassCapture::onCaptured(int status, QString path, QStringList errors)
{
    if(status != QPhoto::QCamera::OK) {
        m_problem = CameraProblem;
        m_morphology->cancelTurnMessage()->launch();
        m_nextAnglePhoto -= m_step; // last pic failed
        emit problem(CameraProblem, errors.join("\n"));
    } else {
        qDebug() << (m_nextAnglePhoto)/m_step-1;
        qDebug() << path;
        emit progress((m_nextAnglePhoto)/m_step-1, path);
    }
}

void MassCapture::onRotationDone(bool success)
{
    if(m_problem == NoProblem && success)
        emit done();
    else if(m_problem == NoProblem && !success) {
        m_problem = RotationProblem;
        emit problem(RotationProblem, "");
    }
}

void MassCapture::resume()
{
    if(m_camera && m_camera->isConnected()) {
        m_problem = NoProblem;
        Transaction *watcher = m_morphology->completeTurnMessage(m_rotationTime, m_currentAngle);
        connect(watcher, &Transaction::done, this, &MassCapture::onRotationDone);
        watcher->launch();
    } else {
        m_problem = CameraProblem;
        emit problem(CameraProblem, tr("Camera seems to be disconnected."));
    }
}
