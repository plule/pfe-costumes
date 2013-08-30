#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QDialog>
#include <QColorDialog>
#include <QList>
#include <QSettings>
#include <QMessageBox>
#include <QGuiApplication>
#include <QCursor>
#include <QTimer>
#include "camerahandler/camerahandler.h"
#include "common/arduinocommunication.h"
#include "includes/settings.h"
#include "qextserialenumerator.h"

namespace Ui {
class SettingsForm;
}

class SettingsForm : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsForm(QPhoto::CameraHandler *m_handler, ArduinoCommunication *xbee, QWidget *parent = 0);
    ~SettingsForm();

    QPhoto::QCamera *getCamera();
    void refreshCameraList();
    QString getXbeePort();
    int getModelWidth();
    int getModelDepth();

signals:
    void cameraChanged(QPhoto::QCamera *m_camera);
    void xbeePortChanged(QString port);
    void modelWidthChanged(int modelWidth);
    void modelDepthChanged(int modelDepth);
    void backgroundColorChanged(QColor color);
    
public slots:
    void apply();
    void cancel();

private slots:
    void on_detectCamerasButton_clicked();

    void on_detectPortsButton_clicked();

    void on_testPortButton_clicked();

    void on_servoPosSpin_valueChanged(int position);

    void on_closedPositionButton_clicked();

    void on_openedPositionButton_clicked();

    void on_renameButton_clicked();

    void on_turnDurationSpin_valueChanged(double s);

    void on_rotationSpeedSpin_valueChanged(double rpm);

    void on_backgroundColorButton_clicked();

private:
    QString guessXbeePort(QList<QString> candidates);
    QPhoto::QCamera *guessCamera(QList<QPhoto::QCamera*> candidates);
    void selectCurrentCamera();
    void selectCurrentXbeePort();
    QList<QPhoto::QCamera*> fillCameraList();
    QList<QString> fillXBeePortList();
    QList<QString> fillArduinoList();

    Ui::SettingsForm *ui;
    QPhoto::CameraHandler *m_handler;
    ArduinoCommunication *m_xbee;

    QTimer *m_motorSliderTimer;
    bool m_motorSliderChanged;

    QPhoto::QCamera *m_camera;

    QSettings m_settings;
    QString m_xbeePort;

    QColorDialog m_colorDialog;
};

#endif // SETTINGSFORM_H

