#include "settingsform.h"
#include "ui_settingsform.h"

SettingsForm::SettingsForm(QPhoto::CameraHandler *handler, ArduinoCommunication *xbee, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsForm)
{
    ui->setupUi(this);
    this->handler = handler;

    m_xbee = xbee;

    /* XBee ports list */
    xbeePort = guessXbeePort(fillXBeePortList());

    /* Cameras list */
    camera =guessCamera(fillCameraList());

    connect(this, SIGNAL(accepted()), this, SLOT(apply()));
    connect(this, SIGNAL(rejected()), this, SLOT(cancel()));

    selectCurrentCamera();
    selectCurrentXbeePort();
}

SettingsForm::~SettingsForm()
{
    delete ui;
}

QPhoto::QCamera *SettingsForm::getCamera()
{
    return camera;
}

QString SettingsForm::getXbeePort()
{
    return xbeePort;
}

QString SettingsForm::guessXbeePort(QList<QString> candidates)
{
    if(candidates.contains(settings.value("xbeeport").toString()))
        return settings.value("xbeeport").toString();

    foreach(QString candidate, candidates) {
        if(candidate.toLower().contains("usb"))
            return candidate;
    }

    if(candidates.size() > 0)
        return candidates.first();

    return "";
}

QPhoto::QCamera *SettingsForm::guessCamera(QList<QPhoto::QCamera*> candidates)
{
    foreach(QPhoto::QCamera *candidate, candidates)
        if(candidate->getModel() == settings.value("camera").toString())
            return candidate;
    if(candidates.size() > 0)
        return candidates.first();
    return 0;
}

void SettingsForm::selectCurrentCamera()
{
    for(int i=0; i<ui->cameraPortCombo->count(); i++)
        if(ui->cameraPortCombo->itemData(i).value<QPhoto::QCamera*>() == camera)
            ui->cameraPortCombo->setCurrentIndex(i);
}

void SettingsForm::selectCurrentXbeePort()
{
    ui->xbeePortCombo->setCurrentIndex(ui->xbeePortCombo->findData(QVariant(xbeePort)));
}

QList<QPhoto::QCamera *> SettingsForm::fillCameraList()
{
    ui->cameraPortCombo->clear();
    QPhoto::QCamera **cameras;
    int nbCamera;
    nbCamera = handler->getCameras(&cameras);
    QList<QPhoto::QCamera*> cameraList = QList<QPhoto::QCamera*>();
    for(int i=0; i<nbCamera; i++) {
        QVariant variant;
        variant.setValue<QPhoto::QCamera*>(cameras[i]);
        ui->cameraPortCombo->addItem(cameras[i]->getModel(),variant);
        cameraList.append(cameras[i]);
    }
    return cameraList;
}

QList<QString> SettingsForm::fillXBeePortList()
{
    QList<QString> xbeePortList;
    foreach(QextPortInfo portInfo, QextSerialEnumerator::getPorts()) {
        ui->xbeePortCombo->addItem(portInfo.friendName + " (" + portInfo.portName +")",QVariant(portInfo.physName));
        xbeePortList.append(portInfo.physName);
    }
    return xbeePortList;
}

void SettingsForm::apply()
{
    QPhoto::QCamera *newCamera = ui->cameraPortCombo->itemData(ui->cameraPortCombo->currentIndex()).value<QPhoto::QCamera*>();
    if(camera != newCamera) {
        camera = newCamera;
        emit cameraChanged(camera);
    }

    QString newXbeePort = ui->xbeePortCombo->itemData(ui->xbeePortCombo->currentIndex()).toString();
    if(xbeePort != newXbeePort) {
        xbeePort = newXbeePort;
        emit xbeePortChanged(xbeePort);
    }

    if(camera != 0)
        settings.setValue("camera", camera->getModel());
    if(xbeePort != "")
        settings.setValue("xbeeport", xbeePort);
}

void SettingsForm::cancel()
{
    selectCurrentCamera();
    selectCurrentXbeePort();
}

void SettingsForm::on_detectCamerasButton_clicked()
{
    handler->refreshCameraList();
    camera = guessCamera(fillCameraList());
    emit cameraChanged(camera);
}

void SettingsForm::on_detectPortsButton_clicked()
{
    QString newPort = guessXbeePort(fillXBeePortList());
    if(newPort != xbeePort) {
        xbeePort = newPort;
        emit xbeePortChanged(xbeePort);
    }
}

void SettingsForm::on_testPortButton_clicked()
{
    QString port = ui->xbeePortCombo->itemData(ui->xbeePortCombo->currentIndex()).toString();
    QMessageBox msg;
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if(m_xbee->testPort(port)) {
        msg.setIcon(QMessageBox::Information);
        msg.setText(tr("The port seems to be valid"));
    } else {
        msg.setIcon(QMessageBox::Warning);
        msg.setText(tr("The port seems to be invalid"));
    }
    QGuiApplication::restoreOverrideCursor();
    msg.exec();
}
