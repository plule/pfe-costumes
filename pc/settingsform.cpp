#include "settingsform.h"
#include "ui_settingsform.h"

SettingsForm::SettingsForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsForm)
{
    ui->setupUi(this);

    /* XBee ports list */
    foreach(QextPortInfo portInfo, QextSerialEnumerator::getPorts()) {
        /*QVariant variant;
        variant.setValue<QextPortInfo>(portInfo);*/
        ui->xbeePortCombo->addItem(portInfo.friendName + " (" + portInfo.portName +")");
    }

    /* Cameras list */
    handler = new QPhoto::CameraHandler();
    QPhoto::QCamera **cameras;
    int nbCamera;
    nbCamera = handler->getCameras(&cameras);
    for(int i=0; i<nbCamera; i++) {
        ui->cameraPortCombo->addItem(cameras[i]->getSummary());
    }
}

SettingsForm::~SettingsForm()
{
    delete ui;
}
