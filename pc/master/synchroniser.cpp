#include "synchroniser.h"

Synchroniser::Synchroniser(QObject *parent) :
    QObject(parent)
{
}

void Synchroniser::massCapture(QPhoto::QCamera *camera, Morphology *morphology, int nbPhoto)
{
    qDebug() << "mass capture";
    m_step = 360.0 / nbPhoto;
    m_actionNumber = 0;
    m_target = nbPhoto;
    m_camera = camera;
    m_morphology = morphology;

    connect(morphology, SIGNAL(done()), this, SLOT(onRotationDone()));
    m_morphology->setRotation(0);
}

void Synchroniser::onCaptureDone()
{
    qDebug() << "capture done";
    m_actionNumber++;
    if(m_actionNumber == m_target) {
        m_morphology->setRotation(0);
        emit done();
    } else
        m_morphology->setRotation(m_step*m_actionNumber);
}

void Synchroniser::onRotationDone()
{
    qDebug() << "rotation done";
    // do capture
    onCaptureDone();
}
