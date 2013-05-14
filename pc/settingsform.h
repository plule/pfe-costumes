#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QDialog>
#include "camerahandler/camerahandler.h"
#include "qextserialenumerator.h"

namespace Ui {
class SettingsForm;
}

class SettingsForm : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsForm(QWidget *parent = 0);
    ~SettingsForm();
    
private:
    Ui::SettingsForm *ui;
    QPhoto::CameraHandler *handler;
};

#endif // SETTINGSFORM_H
