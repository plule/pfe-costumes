#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMetaObject>
#include <QDir>
#include <QPixmap>
#include <QLabel>
#include <QTimer>
#include <QList>
#include <QPair>
#include <QErrorMessage>
#include <QSettings>
#include <QStringList>
#include <QFormLayout>
#include <QLayout>
#include <QMap>
#include <QLineEdit>
#include <QSpinBox>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QPushButton>
#include <QItemSelectionModel>
#include <QDataWidgetMapper>
#include <QSqlQueryModel>
#include "common/qturntable.h"
#include "camerahandler/camerahandler.h"
#include "camerahandler/qcamera.h"
#include "common/slotlog.h"
#include "common/collection.h"

#define GROUP_INFO "costume_informations"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void refresh();
    void startWork(QString work, int target);
    void updateStatusBar(QString message);
    void displayError(QString error);
    
private slots:
    void on_captureButton_clicked();

    void on_refreshButton_clicked();
    void timeout();

    void on_suzanneButton_pressed();

    void on_actionNew_Collection_triggered();

    void on_actionOpen_Collection_triggered();

    void on_loadButton_clicked();

    void on_newCostume_clicked();

    void on_removeButton_clicked();

    void on_saveButton_clicked();

private:
    Ui::MainWindow *ui;
    QPhoto::CameraHandler *handler;
    SlotLog *logger;
    QList<QPixmap> *pics;
    QErrorMessage errorMessage;
    QDataWidgetMapper mapper;
    Collection *collection;
    QSettings settings;

    void doCamerasConnections();
    void loadCollection(QString path);
};

#endif // MAINWINDOW_H
