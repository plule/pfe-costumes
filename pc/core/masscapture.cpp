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
    m_target = nbPhoto;
    setCamera(camera);
    m_morphology = morphology;
    m_collection = collection;
    m_idCostume = idCostume;
    m_problem = NoProblem;
    m_rotationTime = round(60.0 / m_settings.value(S_RPM).toDouble());
    m_captureTimer = new QTimer(this);

    if(m_settings.value(S_AUTOMATEDROTATION).toBool()) {
        m_morphology->completeTurnMessage(m_rotationTime);
        QTimer *delayStart = new QTimer(this);
        delayStart->setSingleShot(true);
        delayStart->setInterval(m_settings.value(S_ROTATIONDELAY).toInt()*1000);
        connect(delayStart, &QTimer::timeout, this, &MassCapture::launchMassCapture);
        delayStart->start();
    } else {
        launchMassCapture();
    }
}

void MassCapture::setCamera(QPhoto::QCamera *camera)
{
    if(camera != 0) {
        connect(camera, &QPhoto::QCamera::destroyed, [=]() {
            m_camera = 0;
        });
        m_camera = camera;
    }
}

void MassCapture::launchMassCapture()
{
    m_index = 0;
    m_captureTimer->setSingleShot(false);
    m_captureTimer->setInterval((1000 * m_rotationTime) / m_settings.value(S_PHOTONUMBER).toInt()); // delay between each shot in ms

    m_camera->captureToFile("/tmp/dummy.jpg"); // ensure camera is initialized

    connect(m_camera, &QPhoto::QCamera::finished, this, &MassCapture::onCaptured);
    connect(m_captureTimer, &QTimer::timeout, [=]() {
        if(m_index >= m_target) {
            m_captureTimer->stop();
            if(m_settings.value(S_AUTOMATEDROTATION).toBool())
                m_morphology->cancelTurnMessage()->launch();
            emit done();
        } else if(m_camera && m_camera->isConnected()) {
            m_index++;
            QString path = m_collection->getFilePath(m_idCostume, "turntable", m_settings.value(S_RAWEXTENSION).toString(), m_index);
            m_pathIndex.insert(path, m_index);
            m_camera->captureToFile(path);
        } else {
            m_morphology->cancelTurnMessage()->launch();
            emit problem(CameraProblem, tr("Camera seems to be disconnected"));
        }
    });
    m_pathIndex.clear();
    m_captureTimer->start();
}

void MassCapture::onCaptured(int status, QString path, QStringList errors)
{
    qDebug() << "Captured " << path << " with status " << QString::number(status);
    if(status != QPhoto::QCamera::OK) {
        m_problem = CameraProblem;
        m_captureTimer->stop(); // stop trying to capture photos
        m_morphology->cancelTurnMessage()->launch();
        m_index--;
        emit problem(CameraProblem, errors.join("\n"));
    } else if(m_pathIndex.contains(path)) {
        emit progress(m_pathIndex.value(path)-1, path);
    } else {
        qWarning() << "Camera captured a photo for an unknown reason";
    }
}

void MassCapture::resume()
{
    if(m_camera && m_camera->isConnected()) {
        m_captureTimer->start(); // resume photos capture
        m_problem = NoProblem;
    } else {
        m_problem = CameraProblem;
        emit problem(CameraProblem, tr("Camera seems to be disconnected."));
    }
}
