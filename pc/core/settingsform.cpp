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
    if(!m_settings.value(S_RAWEXTENSION).isValid())
        m_settings.setValue(S_RAWEXTENSION, "nef");
    ui->rawExtensionEdit->setText(m_settings.value(S_RAWEXTENSION).toString());

    /* Number of capture */
    if(!m_settings.value(S_PHOTONUMBER).isValid())
        m_settings.setValue(S_PHOTONUMBER,36);
    ui->captureNumber->setValue(m_settings.value(S_PHOTONUMBER).toInt());

    /* Rotation settings */
    if(!m_settings.value(S_RPM).isValid())
        m_settings.setValue(S_RPM,0.8);
    ui->rotationSpeedSpin->setValue(m_settings.value(S_RPM).toDouble());
    ui->turnDurationSpin->setValue(60.0/m_settings.value(S_RPM).toDouble());

    /* Model dimension */
    if(!m_settings.value(S_MODELWIDTH).isValid())
        m_settings.setValue(S_MODELWIDTH,100);
    ui->modelWidthBox->setValue(m_settings.value(S_MODELWIDTH).toInt());

    if(!m_settings.value(S_MODELDEPTH).isValid())
        m_settings.setValue(S_MODELDEPTH,124);
    ui->modelDepthBox->setValue(m_settings.value(S_MODELDEPTH).toInt());

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
    if(m_settings.value(S_MODELWIDTH).isValid())
        return m_settings.value(S_MODELWIDTH).toInt();
    return 100;
}

int SettingsForm::getModelDepth()
{
    if(m_settings.value(S_MODELDEPTH).isValid())
        return m_settings.value(S_MODELWIDTH).toInt();
    return 124;
}

QString SettingsForm::guessXbeePort(QList<QString> candidates)
{
    if(candidates.contains(m_settings.value(S_XBEEPORT).toString()))
        return m_settings.value(S_XBEEPORT).toString();

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
        if(candidate->getModel() == m_settings.value(S_CAMERA).toString())
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
    if(m_settings.value(S_MODELWIDTH).toInt() != newModelWidth) {
        m_settings.setValue(S_MODELWIDTH, ui->modelWidthBox->value());
        emit modelWidthChanged(newModelWidth);
    }

    int newModelDepth = ui->modelDepthBox->value();
    if(m_settings.value(S_MODELDEPTH).toInt() != newModelDepth) {
        m_settings.setValue(S_MODELDEPTH, ui->modelDepthBox->value());
        emit modelDepthChanged(newModelDepth);
    }

    if(m_camera != 0)
        m_settings.setValue(S_CAMERA, m_camera->getModel());
    if(m_xbeePort != "")
        m_settings.setValue(S_XBEEPORT, m_xbeePort);
    if(ui->rawExtensionEdit->text() != "")
        m_settings.setValue(S_RAWEXTENSION, ui->rawExtensionEdit->text());

    qDebug() << ui->captureNumber->value();
    m_settings.setValue(S_PHOTONUMBER,ui->captureNumber->value());
    m_settings.setValue(S_RPM,ui->rotationSpeedSpin->value());
}

void SettingsForm::cancel()
{
    selectCurrentCamera();
    selectCurrentXbeePort();
    ui->captureNumber->setValue(m_settings.value(S_PHOTONUMBER).toInt());
    ui->modelDepthBox->setValue(m_settings.value(S_MODELDEPTH).toInt());
    ui->modelWidthBox->setValue(m_settings.value(S_MODELWIDTH).toInt());
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

void SettingsForm::on_turnDurationSpin_valueChanged(double s)
{
    if(s > 0)
        ui->rotationSpeedSpin->setValue(60.0/s);
}

void SettingsForm::on_rotationSpeedSpin_valueChanged(double rpm)
{
    if(rpm > 0)
        ui->turnDurationSpin->setValue(60.0/rpm);
}
