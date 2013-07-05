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
    m_camera = guessCamera(fillCameraList());

    /* Raw file extension */
    if(m_settings.value("rawextension").toString() != "")
        ui->rawExtensionEdit->setText(m_settings.value("rawextension").toString());
    else {
        ui->rawExtensionEdit->setText("nef");
        m_settings.setValue("rawextension", "nef");
    }

    /* Number of capture */
    if(!m_settings.value("photonumber").isValid()) {
        ui->captureNumber->setValue(36);
        m_settings.setValue("photonumber",36);
    } else {
        ui->captureNumber->setValue(m_settings.value("photonumber").toInt());
    }

    /* Model dimension */
    if(!m_settings.value("modelwidth").isValid()) {
        ui->modelWidthBox->setValue(100);
        m_settings.setValue("modelwidth",100);
    } else {
        ui->modelWidthBox->setValue(m_settings.value("modelwidth").toInt());
    }

    if(!m_settings.value("modeldepth").isValid()) {
        ui->modelDepthBox->setValue(124);
        m_settings.setValue("modeldepth",100);
    } else {
        ui->modelDepthBox->setValue(m_settings.value("modeldepth").toInt());
    }

    /* Calibration tab */
    ui->modelCalibBox->setModel(m_xbee->model());
    for(int i = 0; i < m_xbee->getMotorsNumber(); i++) {
        QString type = (m_xbee->getMotorType(i) == FRONT_MOTOR ? tr("Front") : tr("Side"));
        ui->motorCalibBox->addItem(QString("%1 (%2)").arg(m_xbee->getMotorName(i),type),i);
    }

    /* Calibration slider timer to limit number of messages */
    m_motorSliderTimer = new QTimer(this);
    m_motorSliderChanged = false;
    connect(m_motorSliderTimer, &QTimer::timeout, [=]() {
        if(m_motorSliderChanged) {
            m_motorSliderChanged = false;
            m_xbee->setRawMotorPosition(
                        ui->modelCalibBox->itemData(ui->modelCalibBox->currentIndex()).toString(),
                        ui->motorCalibBox->itemData(ui->motorCalibBox->currentIndex()).toInt(),
                        ui->servoPosSpin->value())->launch();
        }
    });
    m_motorSliderTimer->start(100);

    connect(this, SIGNAL(accepted()), this, SLOT(apply()));
    connect(this, SIGNAL(rejected()), this, SLOT(cancel()));

    selectCurrentCamera();
    selectCurrentXbeePort();
}

SettingsForm::~SettingsForm()
{
    delete ui;
    delete m_motorSliderTimer;
}

QPhoto::QCamera *SettingsForm::getCamera()
{
    return m_camera;
}

void SettingsForm::refreshCameraList()
{
    m_handler->refreshCameraList();
    m_camera = guessCamera(fillCameraList());
    emit cameraChanged(m_camera);
}

QString SettingsForm::getXbeePort()
{
    return m_xbeePort;
}

int SettingsForm::getModelWidth()
{
    if(m_settings.value("modelwidth").isValid())
        return m_settings.value("modelwidth").toInt();
    return 100;
}

int SettingsForm::getModelDepth()
{
    if(m_settings.value("modeldepth").isValid())
        return m_settings.value("modeldepth").toInt();
    return 124;
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
    ui->xbeePortCombo->clear();
    foreach(QextPortInfo portInfo, QextSerialEnumerator::getPorts()) {
        ui->xbeePortCombo->addItem(portInfo.friendName + " (" + portInfo.portName +")",QVariant(portInfo.physName));
        xbeePortList.append(portInfo.physName);
    }
    return xbeePortList;
}

QList<QString> SettingsForm::fillArduinoList()
{
    QList<QString> arduinoList = m_xbee->listModel();
    ui->modelCalibBox->clear();
    foreach(QString arduino, arduinoList) {
        ui->modelCalibBox->addItem(arduino);
    }
    return arduinoList;
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

    int newModelWidth = ui->modelWidthBox->value();
    if(m_settings.value("modelwidth").toInt() != newModelWidth) {
        m_settings.setValue("modelwidth", ui->modelWidthBox->value());
        emit modelWidthChanged(newModelWidth);
    }

    int newModelDepth = ui->modelDepthBox->value();
    if(m_settings.value("modeldepth").toInt() != newModelDepth) {
        m_settings.setValue("modeldepth", ui->modelDepthBox->value());
        emit modelDepthChanged(newModelDepth);
    }

    if(m_camera != 0)
        m_settings.setValue("camera", m_camera->getModel());
    if(m_xbeePort != "")
        m_settings.setValue("xbeeport", m_xbeePort);
    if(ui->rawExtensionEdit->text() != "")
        m_settings.setValue("rawextension", ui->rawExtensionEdit->text());

    m_settings.setValue("photonumber",ui->captureNumber->value());
}

void SettingsForm::cancel()
{
    selectCurrentCamera();
    selectCurrentXbeePort();
    ui->captureNumber->setValue(m_settings.value("photonumber").toInt());
    ui->modelDepthBox->setValue(m_settings.value("modeldepth").toInt());
    ui->modelWidthBox->setValue(m_settings.value("modelwidth").toInt());
}

void SettingsForm::on_detectCamerasButton_clicked()
{
    refreshCameraList();
}

void SettingsForm::on_detectPortsButton_clicked()
{
    fillXBeePortList();
    QString newPort = guessXbeePort(fillXBeePortList());
    if(newPort != "") {
        ui->xbeePortCombo->setCurrentIndex(ui->xbeePortCombo->findData(newPort));
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

void SettingsForm::on_servoPosSpin_valueChanged(int position)
{
    m_motorSliderChanged = true;
}

void SettingsForm::on_closedPositionButton_clicked()
{
    m_xbee->setClosePosition(
                ui->modelCalibBox->itemData(ui->modelCalibBox->currentIndex()).toString(),
                ui->motorCalibBox->itemData(ui->motorCalibBox->currentIndex()).toInt(),
                ui->servoPosSlider->value())->launch();
}

void SettingsForm::on_openedPositionButton_clicked()
{
    m_xbee->setOpenPosition(
                ui->modelCalibBox->itemData(ui->modelCalibBox->currentIndex()).toString(),
                ui->motorCalibBox->itemData(ui->motorCalibBox->currentIndex()).toInt(),
                ui->servoPosSlider->value())->launch();
}

void SettingsForm::on_renameButton_clicked()
{
    Transaction *transaction = m_xbee->renameMessage(ui->modelCalibBox->itemData(ui->modelCalibBox->currentIndex()).toString(), ui->renameEdit->text());
    connect(transaction, &Transaction::finished, [=](int id){
        m_xbee->helloMessage()->launch();
    });
    transaction->launch();
}
