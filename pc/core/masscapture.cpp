#include "masscapture.h"

MassCapture::MassCapture(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<MassCapture::Problem>( "MassCapture::Problem" );
}

MassCapture::~MassCapture()
{
    // Avoid risks of having asynchronous called to destroyed object
    QObject::disconnect(m_cameraConnection);
    QObject::disconnect(m_timerConnection);
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
    if(m_camera->captureToFile("/tmp/dummy.jpg", 3) >= 0) // ensure camera is initialized
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
    qDebug() << "launching mass capture";

    // List of photo index to capture
    m_captureIndexes.clear();
    for(int i=0; i<m_target; i++)
        m_captureIndexes.insert(i);

    // Position of the turntable
    m_captureIndex = 0;
    m_currentIndex = 0;

    // Prepare the capture timer
    m_captureTimer->setSingleShot(false);
    m_captureTimer->setInterval((1000 * m_rotationTime) / m_settings.value(S_PHOTONUMBER).toInt()); // delay between each shot in ms

    // Track beginning and end of a capture
    m_readyForNextCapture = true;

    // Timer called at each interesting capture position
    m_timerConnection = connect(m_captureTimer, &QTimer::timeout, [=]() {
        // Update turntable position
        m_currentIndex = (m_currentIndex + 1)%m_target;

        qDebug() << "Turntable position : " << QString::number(m_currentIndex);

        // The turntable is at a capture position, launch the capture if the camera
        // is ready and this shot has not yet been done
        if(m_readyForNextCapture && m_captureIndexes.contains(m_currentIndex)) {
            m_readyForNextCapture = false;
            qDebug() << "Do the capture.";
            m_captureIndex = m_currentIndex;
            m_lastCapturePath = m_collection->getFilePath(m_idCostume, "turntable", "jpg", m_captureIndex);
            if(m_camera && m_camera->isConnected())
                QtConcurrent::run(m_camera, &QPhoto::QCamera::captureToFile, m_lastCapturePath, 3);
            else {
                m_captureTimer->stop();
                emit problem(CameraProblem, tr("Camera seems to be disconnected"));
            }
        }
        else
            qDebug() << "Don't do the capture";
    });

    // At each downloaded picture, report progress and remove captured index from the todo list
    m_cameraConnection = connect(m_camera, &QPhoto::QCamera::downloaded, [=](int status, QStringList errors) {
        if(status == GP_OK) {
            m_captureIndexes.remove(m_captureIndex);

            emit progress(m_target - m_captureIndexes.count() - 1, m_lastCapturePath);

            if(m_captureIndexes.empty()) {
                m_captureTimer->stop();
                emit done();
            }
            else
                m_readyForNextCapture = true;
        }
        else {
            m_captureTimer->stop();
            emit problem(CameraProblem, tr("Capture failed"));
        }
    });

    m_captureTimer->start();
}
