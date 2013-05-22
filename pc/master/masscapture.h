#ifndef SYNCHRONISER_H
#define SYNCHRONISER_H

#include <QObject>
#include <QSettings>
#include <QStringList>
#include "../camerahandler/qcamera.h"
#include "../common/communication.h"
#include "../common/collection.h"

class MassCapture : public QObject
{
    Q_OBJECT
public:
    explicit MassCapture(QObject *parent = 0);
    void massCapture(QPhoto::QCamera *camera, ArduinoCommunication *morphology, Collection *collection, int idCostume, int nbPhoto);
    
signals:
    void progress(int value, QString photoPath);
    void done(bool success);

public slots:

private slots:
    void onCaptureDone(int status, QString path, QStringList errors);
    void onRotationDone(bool success);

private:
    int m_actionNumber;
    int m_target;
    float m_step;
    QPhoto::QCamera *m_camera;
    ArduinoCommunication *m_morphology;
    Collection *m_collection;
    QSettings m_settings;
    int m_idCostume;
};

#endif // SYNCHRONISER_H
