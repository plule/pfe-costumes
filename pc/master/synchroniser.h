#ifndef SYNCHRONISER_H
#define SYNCHRONISER_H

#include <QObject>
#include "../camerahandler/qcamera.h"
#include "../common/morphology.h"
#include "../common/collection.h"

class Synchroniser : public QObject
{
    Q_OBJECT
public:
    explicit Synchroniser(QObject *parent = 0);
    void massCapture(QPhoto::QCamera *camera, Morphology *morphology, Collection *collection, int idCostume, int nbPhoto);
    
signals:
    void done();

public slots:
    void onCaptureDone();
    void onRotationDone();

private slots:

private:
    int m_actionNumber;
    int m_target;
    float m_step;
    QPhoto::QCamera *m_camera;
    Morphology *m_morphology;
    Collection *m_collection;
    int m_idCostume;
};

#endif // SYNCHRONISER_H
