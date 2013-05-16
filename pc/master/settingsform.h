#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QDialog>
#include <QList>
#include <QSettings>
#include <QMessageBox>
#include <QGuiApplication>
#include <QCursor>
#include "camerahandler/camerahandler.h"
#include "common/communication.h"
#include "qextserialenumerator.h"

namespace Ui {
class SettingsForm;
}

class SettingsForm : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsForm(QPhoto::CameraHandler *handler, ArduinoCommunication *xbee, QWidget *parent = 0);
    ~SettingsForm();

    QPhoto::QCamera *getCamera();
    QString getXbeePort();

signals:
    void cameraChanged(QPhoto::QCamera *camera);
    void xbeePortChanged(QString port);
    
public slots:
    void apply();
    void cancel();

private slots:
    void on_detectCamerasButton_clicked();

    void on_detectPortsButton_clicked();

    void on_testPortButton_clicked();

private:
    QString guessXbeePort(QList<QString> candidates);
    QPhoto::QCamera *guessCamera(QList<QPhoto::QCamera*> candidates);
    void selectCurrentCamera();
    void selectCurrentXbeePort();
    QList<QPhoto::QCamera*> fillCameraList();
    QList<QString> fillXBeePortList();

    Ui::SettingsForm *ui;
    QPhoto::CameraHandler *handler;
    ArduinoCommunication *m_xbee;

    QPhoto::QCamera *camera;

    QSettings settings;
    QString xbeePort;
};

#endif // SETTINGSFORM_H
