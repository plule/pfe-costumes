#include "masscapture.h"

MassCapture::MassCapture(QObject *parent) :
    QObject(parent)
{
}

MassCapture::~MassCapture()
{
}

void MassCapture::massCapture(QPhoto::QCamera *camera, ArduinoCommunication *morphology, Collection *collection, int idCostume, int nbPhoto, bool delayedDownload)
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
    m_delayedDownload = delayedDownload;

    qDebug() << "Mass Capture object created";
    if(m_camera->captureToFile("/tmp/dummy.jpg") >= 0) // ensure camera is initialized
        launchMassCapture();
    else {
        qDebug() << "failed to initialize camera";
        emit problem(CameraProblem, tr("Camera seems disconnected."));
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
    qDebug() << "camera synced and capture launched";
    m_index = 0;
    m_captureTimer->setSingleShot(false);
    m_captureTimer->setInterval((1000 * m_rotationTime) / m_settings.value(S_PHOTONUMBER).toInt()); // delay between each shot in ms

    connect(m_captureTimer, &QTimer::timeout, [=]() {
        qDebug() << "launching capture";
        if(m_index >= m_target) {
            qDebug() << "capture finished without problem";
            m_captureTimer->stop();

            if(m_delayedDownload) {
                qDebug() << "downloading";
                for(int i = 0; i < m_cameraPaths.size(); i++) {
                    QString localPath = m_collection->getFilePath(m_idCostume, "turntable", "jpg", i);
                    QFile localFile(localPath);
                    qDebug() << localPath;
                    qDebug() << m_cameraPaths.value(i);
                    m_camera->getFile(m_cameraPaths.value(i).first, m_cameraPaths.value(i).second, &localFile);
                    emit progress(i, localPath);
                }
            }

            emit done();
        } else if(m_camera && m_camera->isConnected()) {
            qDebug() << "capture n°" << QString::number(m_index);
            int ret;
            QString path = "";
            if(!m_delayedDownload) {
                path = m_collection->getFilePath(m_idCostume, "turntable", "jpg", m_index);
                ret = m_camera->captureToFile(path);
            } else {
                qDebug() << "capture to camera";
                QPair<QString,QString> cameraPath = m_camera->captureToCamera();
                qDebug() << cameraPath;
                if(cameraPath.first != "") {
                    m_cameraPaths.insert(m_index,cameraPath);
                    ret = GP_OK;
                } else {
                    ret = -1;
                }
            }
            if(ret < 0) {
                qDebug() << "Mass capture failed";
                m_captureTimer->stop();
                emit problem(CameraProblem, tr("Capture failed"));
            } else {
                emit progress(m_index++, path);
            }
        } else {
            m_captureTimer->stop();
            qDebug() << "camera problem";
            emit problem(CameraProblem, tr("Camera seems to be disconnected"));
        }
        qDebug() << "exiting timer timeout routine";
    });
    m_captureTimer->start();
}
