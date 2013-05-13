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

    connect(morphology, SIGNAL(done()), this, SLOT(onRotationDone()));
    connect(camera, SIGNAL(captured(QString)), this, SLOT(onCaptureDone()));
    connect(camera, SIGNAL(operation_failed(QString)), this, SLOT(onCaptureFail()));
    m_morphology->setRotation(0);
}

void Synchroniser::onCaptureDone()
{
    qDebug() << "capture done";
    m_actionNumber++;
    if(m_actionNumber == m_target) {
        m_morphology->setRotation(0);
        emit done(true);
    } else
        m_morphology->setRotation(m_step*m_actionNumber);
}

void Synchroniser::onCaptureFail()
{
    emit done(false);
}

void Synchroniser::onRotationDone()
{
    qDebug() << "rotation done";
    QString path = m_collection->getNewFilePath(m_idCostume, "turntable", "jpg"); // TODO extension follow config
    m_camera->captureToFile(path);
}
