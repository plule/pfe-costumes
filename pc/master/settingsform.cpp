#include "settingsform.h"
#include "ui_settingsform.h"

SettingsForm::SettingsForm(QPhoto::CameraHandler *handler, ArduinoCommunication *xbee, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsForm)
{
    ui->setupUi(this);
    this->m_handler = handler;

    m_xbee = xbee;

    /* XBee ports list */
    m_xbeePort = guessXbeePort(fillXBeePortList());

    /* Cameras list */
    m_camera =guessCamera(fillCameraList());

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
    return m_camera;
}

QString SettingsForm::getXbeePort()
{
    return m_xbeePort;
}

QString SettingsForm::guessXbeePort(QList<QString> candidates)
{
    if(candidates.contains(m_settings.value("xbeeport").toString()))
        return m_settings.value("xbeeport").toString();

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
        if(candidate->getModel() == m_settings.value("camera").toString())
            return candidate;
    if(candidates.size() > 0)
        return candidates.first();
    return 0;
}

void SettingsForm::selectCurrentCamera()
{
    for(int i=0; i<ui->cameraPortCombo->count(); i++)
        if(ui->cameraPortCombo->itemData(i).value<QPhoto::QCamera*>() == m_camera)
            ui->cameraPortCombo->setCurrentIndex(i);
}

void SettingsForm::selectCurrentXbeePort()
{
    ui->xbeePortCombo->setCurrentIndex(ui->xbeePortCombo->findData(QVariant(m_xbeePort)));
}

QList<QPhoto::QCamera *> SettingsForm::fillCameraList()
{
    ui->cameraPortCombo->clear();
    QPhoto::QCamera **cameras;
    int nbCamera;
    nbCamera = m_handler->getCameras(&cameras);
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
    if(m_camera != newCamera) {
        m_camera = newCamera;
        emit cameraChanged(m_camera);
    }

    QString newXbeePort = ui->xbeePortCombo->itemData(ui->xbeePortCombo->currentIndex()).toString();
    if(m_xbeePort != newXbeePort) {
        m_xbeePort = newXbeePort;
        emit xbeePortChanged(m_xbeePort);
    }

    if(m_camera != 0)
        m_settings.setValue("camera", m_camera->getModel());
    if(m_xbeePort != "")
        m_settings.setValue("xbeeport", m_xbeePort);
}

void SettingsForm::cancel()
{
    selectCurrentCamera();
    selectCurrentXbeePort();
}

void SettingsForm::on_detectCamerasButton_clicked()
{
    m_handler->refreshCameraList();
    m_camera = guessCamera(fillCameraList());
    emit cameraChanged(m_camera);
}

void SettingsForm::on_detectPortsButton_clicked()
{
    QString newPort = guessXbeePort(fillXBeePortList());
    if(newPort != m_xbeePort) {
        m_xbeePort = newPort;
        emit xbeePortChanged(m_xbeePort);
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
