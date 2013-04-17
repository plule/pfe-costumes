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
#include <QErrorMessage>
#include <QSettings>
#include <QStringList>
#include <QFormLayout>
#include <QLayout>
#include <QMap>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QFileDialog>
#include <QPushButton>
#include "common/qturntable.h"
#include "camerahandler/camerahandler.h"
#include "camerahandler/qcamera.h"
#include "common/slotlog.h"
#include "common/costume.h"

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

private:
    Ui::MainWindow *ui;
    QPhoto::CameraHandler *handler;
    SlotLog *logger;
    QList<QPixmap> *pics;
    QErrorMessage errorMessage;
    QMap<QString, QWidget*> infoWidgets;

    void doConnections();
    void initInfoLayout(QFormLayout *layout, const QList<Costume_info> infoList);
};

#endif // MAINWINDOW_H
