#include "synchroniser.h"

Synchroniser::Synchroniser(QObject *parent) :
    QObject(parent)
{
}

void Synchroniser::massCapture(QPhoto::QCamera *camera, Morphology *morphology, Collection *collection, int idCostume, int nbPhoto)
{
    qDebug() << "mass capture";
    m_step = 360.0 / nbPhoto;
    m_actionNumber = 0;
    m_target = nbPhoto;
    m_camera = camera;
    m_morphology = morphology;
    m_collection = collection;
    m_idCostume = idCostume;

    connect(camera, SIGNAL(downloading()), this, SLOT(onCaptureDone()));
    connect(camera, SIGNAL(operation_failed(QString)), this, SLOT(onCaptureFail()));

    onCaptureDone();
}

void Synchroniser::onCaptureDone()
{
    qDebug() << "capture done";
    m_actionNumber++;
    if(m_actionNumber > m_target) {
        qDebug() << "mass capture finished";
        this->deleteLater();
        emit done(true);
    } else {
        MessageWatcher *watcher = m_morphology->setRotation(m_step*m_actionNumber);
        connect(watcher, SIGNAL(done(bool)), this, SLOT(onRotationDone(bool)));
    }
}

void Synchroniser::onCaptureFail()
{
    qDebug() << "sync fail";
    emit done(false);
}

void Synchroniser::onRotationDone(bool success)
{
    qDebug() << "rotation done";
    if(success) {
        QString path = m_collection->getNewFilePath(m_idCostume, "turntable", "jpg"); // TODO extension follow config
        m_camera->captureToFile(path);
    } else {
        MessageWatcher *watcher = m_morphology->setRotation(m_step*m_actionNumber);
        connect(watcher, SIGNAL(done(bool)), this, SLOT(onRotationDone(bool)));
    }
}
