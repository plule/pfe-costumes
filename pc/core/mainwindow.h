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
#include "widgets/qellipseslider.h"
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
    void displayError(QString error, QString details = "");
    QString convertRaw(QString path);
    void updateSaveButton();
    void onModelDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);
    void populateList();
    QString getCurrentArduino();
    void setCamera(QPhoto::QCamera *m_camera);
    void onMassCaptureProblem(MassCapture::Problem problem, QString description);
    
private slots:
    void setMotorDistance(QString arduino, int motor, int distance);
    void onCaptureDone(int status, QString path, QStringList errorList);

    void on_captureButton_clicked();

    void on_suzanneButton_pressed();

    void on_actionNew_Collection_triggered();

    void on_actionOpen_Collection_triggered();

    void on_newCostume_clicked();

    void on_removeButton_clicked();

    void on_saveButton_clicked();

    void on_manButton_clicked();

    void on_massCaptureButton_clicked();

    void on_actionSettings_triggered();

    void on_rotateToViewButton_clicked();

private:

    Ui::MainWindow *ui;
    SettingsForm *m_settingsForm;
    QProgressDialog *m_progressDialog;
    QPhoto::CameraHandler *m_handler;
    SlotLog *m_logger;
    QList<QPixmap> *m_pics;
    QErrorMessage m_errorMessage;
    QDataWidgetMapper m_mapper;
    Collection *m_collection;
    ArduinoCommunication *m_arduinoCommunication;
    QSettings m_settings;
    int m_currentCostumeId;
    QHash<int,QEllipseSlider *> m_frontSliders;
    QHash<int,QEllipseSlider *> m_sideSliders;
    DcRawQT m_rawHandler;
    QPhoto::QCamera *m_camera;
    QMetaObject::Connection m_cameraConnection;

    void loadCollection(QString path);
    int getCurrentId();
    bool saveDialog();
};

#endif // MAINWINDOW_H
