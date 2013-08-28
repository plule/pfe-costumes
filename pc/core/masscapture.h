#ifndef SYNCHRONISER_H
#define SYNCHRONISER_H

#include <QObject>
#include <QSettings>
#include <QStringList>
#include <cmath>
#include <QTemporaryFile>
#include <QThread>
#include "camerahandler/qcamera.h"
#include "common/arduinocommunication.h"
#include "common/collection.h"
#include "includes/settings.h"

/**
 * @brief Synchronise an arduino and a camera to take photos.
 */
class MassCapture : public QObject
{
    Q_OBJECT
    Q_ENUMS(Problem)


public:
    explicit MassCapture(QObject *parent = 0);
    ~MassCapture();
    void massCapture(QPhoto::QCamera *camera, ArduinoCommunication *morphology, Collection *collection, int idCostume, int nbPhoto);
    void setCamera(QPhoto::QCamera *camera);
    enum Problem {CameraProblem, RotationProblem, NoProblem};
    
signals:
    // Indicate that a new photo has been taken. Value is between 0 and nbPhoto-1
    void progress(int value, QString photoPath);
    // Indicate that work has been done with success
    void done();
    // Indicate that a problem happened. Execution is stopped and the object can be destroyed.
    void problem(MassCapture::Problem problem, QString description);

private slots:
    void launchMassCapture();

private:
    int m_index;
    int m_target;
    float m_step;
    int m_rotationTime;
    int m_currentAngle;
    QPhoto::QCamera *m_camera;
    ArduinoCommunication *m_morphology;
    Collection *m_collection;
    QSettings m_settings;
    Problem m_problem;
    QTimer *m_captureTimer;
    int m_idCostume;
};

#endif // SYNCHRONISER_H
