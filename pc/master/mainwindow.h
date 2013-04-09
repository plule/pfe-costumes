#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMetaObject>
#include "cameraHandler/cameraHandler.h"
#include "cameraHandler/QCamera.h"
#include "common/SlotLog.h"
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
    
private slots:
    void on_captureButton_clicked();

    void on_refreshButton_clicked();

private:
    Ui::MainWindow *ui;
    CameraHandler *handler;
    SlotLog *logger;
};

#endif // MAINWINDOW_H
