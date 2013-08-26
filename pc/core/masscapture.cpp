#include "masscapture.h"

MassCapture::MassCapture(QObject *parent) :
    QObject(parent)
{
}

MassCapture::~MassCapture()
{
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

    qDebug() << "Mass Capture object created";
    connect(m_camera, &QPhoto::QCamera::finished, this, &MassCapture::launchMassCapture);
    m_camera->captureToFile("/tmp/dummy.jpg"); // ensure camera is initialized
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
    disconnect(m_camera, 0, this, 0);
    qDebug() << "camera synced and capture launched";
    m_index = 0;
    m_captureTimer->setSingleShot(false);
    m_captureTimer->setInterval((1000 * m_rotationTime) / m_settings.value(S_PHOTONUMBER).toInt()); // delay between each shot in ms

    connect(m_captureTimer, &QTimer::timeout, [=]() {
        qDebug() << "launching capture";
        if(m_index >= m_target) {
            qDebug() << "capture finished without problem";
            m_captureTimer->stop();
            emit done();
        } else if(m_camera && m_camera->isConnected()) {
            m_index++;
            qDebug() << "capture n°" << QString::number(m_index);
            QString path = m_collection->getFilePath(m_idCostume, "turntable", "jpg", m_index);
            m_camera->captureToFile(path);
            emit progress(m_index-1, path);
        } else {
            qDebug() << "camera problem";
            emit problem(CameraProblem, tr("Camera seems to be disconnected"));
        }
        qDebug() << "exiting timer timeout routine";
    });
    m_captureTimer->start();
}
