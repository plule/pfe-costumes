#ifndef SYNCHRONISER_H
#define SYNCHRONISER_H

#include <QObject>
#include <QSettings>
#include <QStringList>
#include "camerahandler/qcamera.h"
#include "common/communication.h"
#include "common/collection.h"

/**
 * @brief Synchronise an arduino and a camera to take photos.
 */
class MassCapture : public QObject
{
    Q_OBJECT
    Q_ENUMS(Problem)


public:
    explicit MassCapture(QObject *parent = 0);
    void massCapture(QPhoto::QCamera *camera, ArduinoCommunication *morphology, Collection *collection, int idCostume, int nbPhoto);
    void setCamera(QPhoto::QCamera *camera);
    enum Problem {CameraProblem, RotationProblem, NoProblem};
    
signals:
    // Indicate that a new photo has been taken. Value is between 0 and nbPhoto-1
    void progress(int value, QString photoPath);
    // Indicate that work has been done with success
    void done();
    // Indicate that a problem happened. Execution is stopped until resume() is called. To cancel, destroy the object
    void problem(MassCapture::Problem problem);

public slots:
    // If a problem happened, try to resume execution
    void resume();


private slots:
    void onCaptureDone(int status, QString path, QStringList errors);
    void onRotationDone(bool success);
    void onAngleChanged(int angle);
    void launchMassCapture();
    void onCaptured(int status, QString path, QStringList errors);

private:
    int m_actionNumber;
    int m_target;
    float m_step;
    float m_nextAnglePhoto;
    QPhoto::QCamera *m_camera;
    ArduinoCommunication *m_morphology;
    Collection *m_collection;
    QSettings m_settings;
    Problem m_problem;
    int m_idCostume;
};

#endif // SYNCHRONISER_H