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
#include "common/collection.h"
#include "common/arduinocommunication.h"
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
    void setCamera(QPhoto::QCamera *m_camera);
    void onMassCaptureProblem(MassCapture::Problem problem, QString description);
    
private slots:
    void on_actionNew_Collection_triggered();

    void on_actionOpen_Collection_triggered();

    void on_newCostume_clicked();

    void on_removeButton_clicked();

    void on_saveButton_clicked();

    void on_massCaptureButton_clicked();

    void on_actionSettings_triggered();

private:
    QWidget *createAdjustmentGroup(QString arduinoId);

    Ui::MainWindow *ui;
    SettingsForm *m_settingsForm;
    QProgressDialog *m_progressDialog;
    QPhoto::CameraHandler *m_handler;
    QList<QPixmap> *m_pics;
    QErrorMessage m_errorMessage;
    QDataWidgetMapper m_mapper;
    Collection *m_collection;
    ArduinoCommunication *m_arduinoCommunication;
    QSettings m_settings;
    int m_currentCostumeId;
    DcRawQT m_rawHandler;
    QPhoto::QCamera *m_camera;
    QMetaObject::Connection m_cameraConnection;
    QTimer *m_motorTimer;
    QHash<QPair<QString,int>,int> m_dirtyMotors; // stores the modified sliders and the asked value
    QMap<QString,QWidget*> m_adjustmentGroups; // stores the adjustment groups by slider's id

    void loadCollection(QString path);
    int getCurrentId();
    bool saveDialog();
};

#endif // MAINWINDOW_H
