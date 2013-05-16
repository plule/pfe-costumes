#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMetaObject>
#include <QDir>
#include <QList>
#include <QPair>
#include <QErrorMessage>
#include <QSettings>
#include <QStringList>
#include <QFormLayout>
#include <QLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QPushButton>
#include <QDataWidgetMapper>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSqlTableModel>
#include <QCheckBox>
#include <QCompleter>
#include <QMessageBox>
#include "settingsform.h"
#include "widgets/qloadedlistwidget.h"
#include "widgets/qturntable.h"
#include "camerahandler/camerahandler.h"
#include "camerahandler/qcamera.h"
#include "common/slotlog.h"
#include "common/collection.h"
#include "common/communication.h"
#include "masscapture.h"
#include "dcrawqt.h"

#define GROUP_INFO "costume_informations"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(int m_currentCostumeId READ getCurrentCostumeId WRITE setCurrentCostumeId USER true)

    enum CaptureAction {Ignore, Replace, Append};
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int getCurrentCostumeId() const;
    void setCurrentCostumeId(int value);

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void startWork(QString work, int target);
    void endWork();
    void updateStatusBar(QString message);
    void displayError(QString error);
    void handleNewPicture(QString path);
    void updateSaveButton();
    void onModelDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);
    void populateList();
    void addDevice(Arduino arduino);
    void removeDevice(Arduino arduino);
    int getCurrentArduino();
    void whenMassCaptureDone(bool success);
    void setCamera(QPhoto::QCamera *m_camera);
    void registerError(QString error);
    void clearErrors();
    
private slots:
    void timeout();
    void sendMs(int ms);
    void setMotorMicroSecond(int arduino,int motor,int ms);

    void on_captureButton_clicked();

    void on_suzanneButton_pressed();

    void on_actionNew_Collection_triggered();

    void on_actionOpen_Collection_triggered();

    void on_newCostume_clicked();

    void on_removeButton_clicked();

    void on_saveButton_clicked();

    void on_manButton_clicked();

    void on_appendCaptureButton_clicked();

    void on_massCaptureButton_clicked();

    void on_actionSettings_triggered();

private:

    Ui::MainWindow *ui;
    SettingsForm *m_settingsForm;
    QPhoto::CameraHandler *m_handler;
    QPhoto::QCamera *m_camera;
    SlotLog *m_logger;
    QList<QPixmap> *m_pics;
    QErrorMessage m_errorMessage;
    QDataWidgetMapper m_mapper;
    Collection *m_collection;
    ArduinoCommunication *m_arduinoCommunication;
    QSettings m_settings;
    QMap <QString, CaptureAction> m_captureActions;
    bool m_massCaptureRunning;
    int m_currentCostumeId;
    QList <QSlider *> m_morphoSliders;
    QStringList m_lastErrors;

    void loadCollection(QString path);
    int getCurrentId();
    bool saveDialog();
};

#endif // MAINWINDOW_H
