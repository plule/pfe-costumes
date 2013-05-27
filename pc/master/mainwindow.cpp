#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_collection = 0;
    m_camera = 0;
    m_progressDialog = new QProgressDialog();
    // Logger that show what goes through the slots
    m_logger = new SlotLog();

    // Ui init and tweaks
    QIcon::setThemeName("elementary-xfce"); // TODO fix this.
    ui->setupUi(this);

    // Handler and arduino communication modules
    m_handler = new QPhoto::CameraHandler();
    m_arduinoCommunication = new ArduinoCommunication(this);

    // Handle cameras (listing, taking photos, etc...)
    connect(m_handler, SIGNAL(message(QString)), this, SLOT(updateStatusBar(QString)));
    m_handler->init();

    // Settings window
    m_settingsForm = new SettingsForm(m_handler, m_arduinoCommunication, this);
    connect(m_settingsForm, SIGNAL(cameraChanged(QPhoto::QCamera*)), this, SLOT(setCamera(QPhoto::QCamera*)));
    setCamera(m_settingsForm->getCamera());

    // Arduino comm configuration and ui init
    m_arduinoCommunication->setPort(m_settingsForm->getXbeePort());
    for(int i=0; i < m_arduinoCommunication->getMotorsNumber(); i++) {
        QString name = QString(m_arduinoCommunication->getMotorsNames()[i]);
        QSlider *slider = new QSlider(Qt::Horizontal, this);
        slider->setMinimum(MORPHO_MIN);
        slider->setMaximum(MORPHO_MAX);
        slider->setProperty("motor", i);

        QSpinBox *spin = new QSpinBox(this);
        spin->setMinimum(MORPHO_MIN);
        spin->setMaximum(MORPHO_MAX);

        QHBoxLayout *layout = new QHBoxLayout();
        layout->addWidget(slider);
        layout->addWidget(spin);

        m_morphoSliders.append(slider);

        connect(slider, SIGNAL(valueChanged(int)), spin, SLOT(setValue(int)));
        connect(spin, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
        connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sendMs(int)));

        ui->adjustementForm->addRow(name, layout);
    }

    connect(m_arduinoCommunication, SIGNAL(motorMicrosecondChanged(QString,int,int)), this, SLOT(setMotorMicroSecond(QString,int,int)));
    connect(m_arduinoCommunication, SIGNAL(arduinoAdded(Arduino)), this, SLOT(addDevice(Arduino)));
    connect(m_arduinoCommunication, SIGNAL(arduinoRemoved(Arduino)), this, SLOT(removeDevice(Arduino)));
    m_arduinoCommunication->helloMessage()->launch();
    connect(m_settingsForm, SIGNAL(xbeePortChanged(QString)), m_arduinoCommunication, SLOT(setPort(QString)));

    // Load last collection
    if(m_settings.value("collection").type() == QVariant::String && QFile::exists(m_settings.value("collection").toString()))
        loadCollection(m_settings.value("collection").toString());
    else
        on_actionNew_Collection_triggered();

    showMaximized();
    ui->turntable->fitInView();
}

// Utility to remove all widgets from a layout
static void clearLayout(QLayout* layout, bool deleteWidgets = true)
{
    while (layout->count() > 0)
    {
        QLayoutItem* item = layout->takeAt(0);
        if (deleteWidgets)
        {
            if (QWidget* widget = item->widget())
                delete widget;
        }
        else if (QLayout* childLayout = item->layout())
            clearLayout(childLayout, deleteWidgets);
        delete item;
    }
}

void MainWindow::loadCollection(QString path)
{
    if(m_collection)
        delete m_collection;
    m_settings.setValue("collection", path);
    m_collection = new Collection(this, path);
    if(m_collection->isValid()) {
        QSqlTableModel *model = m_collection->getCollectionModel();

        // Update save button and titles on data change/sync
        connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
        connect(m_collection, SIGNAL(synchronised()), this, SLOT(updateSaveButton()));
        connect(m_collection, SIGNAL(synchronised()), this, SLOT(populateList())); // ensure sync. TODO useful?
        connect(m_collection, SIGNAL(synchronised()), ui->collectionTable2, SLOT(cleanAll()));

        // Configuration of the mapper between costume info widget and database model
        m_mapper.setModel(model);
        m_mapper.clearMapping();
        m_mapper.setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

        // Connection between list widget and mapper
        connect(&m_mapper, SIGNAL(currentIndexChanged(int)), ui->collectionTable2, SLOT(load(int)));
        connect(ui->collectionTable2, SIGNAL(loadedChanged(int)), &m_mapper, SLOT(setCurrentIndex(int)));

        // Creation of the widgets that contains costumes info
        clearLayout(ui->infoLayout, true);
        QList<QPair<Costume_info, QString> > collectionInfos = m_collection->sortedContent();
        for(int i=0; i < collectionInfos.length(); i++) {
            QString key = collectionInfos.at(i).second;
            Costume_info info = collectionInfos.at(i).first;
            QWidget *widget = 0;
            if(info.type == ShortString) {
                QLineEdit *lineEdit = new QLineEdit(this);
                if(info.autocomplete)
                    lineEdit->setCompleter(m_collection->getCompleter(key));
                widget = lineEdit;

            } else if(info.type == PK)
                m_mapper.addMapping(this, model->record().indexOf(key)); // map the pk with currentCostumeId get/set
            else if(info.type == Number) {
                widget = new QSpinBox(this);
                ((QSpinBox*)widget)->setMaximum(9999);
                ((QSpinBox*)widget)->setMinimum(-9999);
            } else if(info.type == LongString)
                widget = new QPlainTextEdit(this);
            else if(info.type == Bool)
                widget = new QCheckBox(this);
            else if(info.type == Files){
                QFileDialog *dialog = new QFileDialog(this);
                dialog->setDirectory(QDir::home());
                dialog->setFileMode(QFileDialog::ExistingFiles);
                QPushButton *button = new QPushButton(this);
                button->setText(tr("Choose file(s)"));
                connect(button, SIGNAL(clicked()), dialog, SLOT(open()));
                // TODO display selected file(s)
                // TODO better multi-file picker

                ui->infoLayout->addRow(info.name, button);
            } else
                qWarning() << QString("Unknown field type for field ") + info.name;

            if(widget != 0) {
                if(!info.external) {
                    if(info.visible) {
                        QString name = info.name;
                        ui->infoLayout->addRow(name, widget);
                    } else
                        widget->setVisible(false);
                }
                m_mapper.addMapping(widget, model->record().indexOf(key));
            }
        }

        populateList();
        m_mapper.toFirst();
    }
}

int MainWindow::getCurrentId()
{
    return m_currentCostumeId;
}

bool MainWindow::saveDialog()
{
    if(m_collection->isDirty()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Save Collection"),
                                   tr("The collection has unsaved changes.\n"
                                      "Do you want to save the modifications?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if(ret == QMessageBox::Save) {
            m_collection->submit();
            return true;
        } else if(ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::populateList()
{
    QSqlTableModel *model = m_collection->getCollectionModel();
    ui->collectionTable2->clear();
    int idRow = model->fieldIndex("id");
    for(int i=0; i<model->rowCount(); i++) {
        QSqlRecord r = model->record(i);
        QListWidgetItem *item = new QListWidgetItem(m_collection->getName(r));
        item->setData(Qt::UserRole, r.value(idRow));
        item->setIcon(QIcon::fromTheme("x-office-document"));
        ui->collectionTable2->insertItem(i,item);
    }
}

void MainWindow::addDevice(Arduino arduino)
{
    ui->ardListCombo->addItem(arduino.id, arduino.id);
    if(arduino.id == getCurrentArduino())
        m_arduinoCommunication->motorsPositionMessage(arduino.id)->launch();
}

void MainWindow::removeDevice(Arduino arduino)
{
    ui->ardListCombo->removeItem(ui->ardListCombo->findData(arduino.id));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_handler;
    delete m_progressDialog;
}

void MainWindow::startWork(QString work, int target)
{
    this->ui->workBar->setFormat(work + " (%p%)");
    this->ui->workBar->setMaximum(target);
    this->ui->workBar->setValue(0);
}

void MainWindow::endWork()
{
    this->ui->workBar->setFormat("");
    this->ui->workBar->setValue(0);
}

void MainWindow::updateStatusBar(QString message)
{
    if(sender() && sender()->objectName().length() > 0)
        this->statusBar()->showMessage(sender()->objectName() + " : " + message);
    else
        this->statusBar()->showMessage(message);
}

void MainWindow::displayError(QString error, QString details)
{
    QMessageBox msg;
    msg.setIcon(QMessageBox::Warning);
    msg.setText(error);
    if(details != "")
        msg.setDetailedText(details);
    qDebug() << error;
    qDebug() << details;
    msg.exec();
}

QString MainWindow::convertRaw(QString path)
{
    /* Raw conversion */
    QString filename = QFileInfo(path).fileName();
    if(m_rawHandler.IsRaw(path)) {

        //dcrawargs += "dcrawqt";
        //dcrawargs += "-c";
        //dcrawargs += "-W";
        //QByteArray *pixBytes = m_rawHandler.GetImage(path, dcrawargs);
        // TODO option for using real conversion
        QByteArray *pixBytes = m_rawHandler.GetThumbNail(path);
        QPixmap pix;
        pix.loadFromData(*pixBytes);//, "PPM");
        delete pixBytes;
        if(pix.isNull()) {
            QMessageBox::information(this, tr("Conversion error"), tr("Failed to convert raw to jpg. The raw file is conserved."));
            updateSaveButton();
            return path;
        } else {
            QFile file(path+".jpg");
            filename += ".jpg";
            file.open(QIODevice::WriteOnly);

            pix.save(&file);
        }
    }
    return filename;
}

void MainWindow::updateSaveButton()
{
    ui->saveButton->setEnabled(m_collection->isDirty());
}

// Do not expect more than one change at a time.
void MainWindow::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    (void)bottomRight;
    updateSaveButton();
    QListWidgetItem *item = ui->collectionTable2->item(topLeft.row());
    if(item) {
        item->setText(m_collection->getName(item->data(Qt::UserRole).toInt()));
        ui->collectionTable2->setDirty(item, m_collection->getCollectionModel()->isDirty(topLeft));
    }
}

void MainWindow::on_captureButton_clicked()
{
    if(m_camera != 0 && m_camera->isConnected()) {
        QString filename = ui->turntable->getCurrentFileName(); // TODO if empty
        QString path = m_collection->getTempStorageDir(getCurrentId(), "turntable").absoluteFilePath(filename);
        m_cameraConnection =  connect(m_camera, &QPhoto::QCamera::finished, [=](int status, QString path, QStringList errorList){
            disconnect(m_cameraConnection);
            switch(status) {
            case QPhoto::QCamera::OK:
            {
                QString filename = convertRaw(path);
                ui->turntable->setCurrentPicture(filename);
                ui->collectionTable2->setDirty(ui->collectionTable2->loadedItem(), true);
                updateSaveButton();
                break;
            }
            case QPhoto::QCamera::Error:
                displayError(tr("Failed to capture photo."), errorList.join("\n"));
                break;
            case QPhoto::QCamera::Timeout:
                displayError(tr("Lost connection to the camera, you should disconnect and reconnect it."));
                break;
            }
        });

                //this, SLOT(onCameraCapture(int,QString,QStringList)));
        m_camera->captureToFile(path);
    } else {
        m_settingsForm->refreshCameraList();
        setCamera(m_settingsForm->getCamera());
        if(m_camera == 0)
            this->displayError(tr("No camera connected"));
        else
            on_captureButton_clicked();
    }
}

QString MainWindow::getCurrentArduino()
{
    return ui->ardListCombo->itemData(ui->ardListCombo->currentIndex(), Qt::UserRole).toString();
}

void MainWindow::setCamera(QPhoto::QCamera *camera)
{
    if(camera != m_camera) {
        this->m_camera = camera;
        if(camera != 0) {
            connect(camera, SIGNAL(error(QString)), m_logger, SLOT(error(QString)));
            connect(camera, SIGNAL(idle()), m_logger, SLOT(idle()));
            connect(camera, SIGNAL(status(QString)), m_logger, SLOT(message(QString)));
            connect(camera, SIGNAL(message(QString)), m_logger, SLOT(message(QString)));
            connect(camera, SIGNAL(progress_update(int)), m_logger, SLOT(progress_update(int)));
            connect(camera, SIGNAL(progress_start(QString, int)), m_logger, SLOT(progress_start(QString, int)));

            connect(camera, SIGNAL(progress_start(QString,int)), this, SLOT(startWork(QString,int)));
            connect(camera, SIGNAL(progress_update(int)), this->ui->workBar, SLOT(setValue(int)));

            connect(camera, SIGNAL(error(QString)), this->statusBar(), SLOT(showMessage(QString)));
        }
    }
}

void MainWindow::onMassCaptureProblem(MassCapture::Problem problem)
{
    MassCapture *synchroniser = (MassCapture *)sender();
    if(synchroniser != 0) {
        QMessageBox errorDialog;
        if(problem == MassCapture::CameraProblem)
            errorDialog.setText(tr("The camera has encountered a problem."));
        else if(problem == MassCapture::RotationProblem)
            errorDialog.setText(tr("The rotating model has encountered a problem."));
        else
            errorDialog.setText(tr("An unknown problem occured."));
        errorDialog.setInformativeText(tr("Do you wish to try to continue the capture?"));
        errorDialog.setIcon(QMessageBox::Warning);
        errorDialog.setStandardButtons(QMessageBox::Retry | QMessageBox::Abort);
        connect(&errorDialog, SIGNAL(accepted()), synchroniser, SLOT(resume()));
        connect(&errorDialog, SIGNAL(rejected()), synchroniser, SLOT(deleteLater()));
        switch(errorDialog.exec()) {
        case QMessageBox::Retry:
            if(problem == MassCapture::CameraProblem) {
                m_settingsForm->refreshCameraList();
                setCamera(m_settingsForm->getCamera());
                synchroniser->setCamera(m_settingsForm->getCamera());
            }
            synchroniser->resume();
            break;
        case QMessageBox::Abort:
            synchroniser->deleteLater();
            break;
        }
    }
}

void MainWindow::sendMs(int ms)
{
    QString current = getCurrentArduino();
    int motor = sender()->property("motor").toInt();
    m_arduinoCommunication->motorMicrosecondMessage(current, motor, ms)->launch();
}

void MainWindow::setMotorMicroSecond(QString arduino, int motor, int ms)
{
    if(arduino == getCurrentArduino() && motor < m_morphoSliders.size()) {
        m_morphoSliders.at(motor)->setValue(ms);
    }
}

void MainWindow::on_newCostume_clicked()
{
    int newId = m_collection->newCostume();
    QListWidgetItem *item = new QListWidgetItem(tr("New Costume"), ui->collectionTable2);
    item->setData(Qt::UserRole, newId);
    item->setIcon(QIcon::fromTheme("x-office-document"));
    ui->collectionTable2->setDirty(item, true);
    ui->collectionTable2->selectionModel()->clear();
    ui->collectionTable2->setCurrentItem(item);
    m_mapper.toLast();
}

void MainWindow::on_suzanneButton_pressed()
{
    ui->turntable->setNumber(36);
    this->startWork(tr("Loading views"), 36);
    for(int i=1; i<=36; ++i)
    {
        QString filename = QString("%1.jpg").arg(QString::number(i), 3, QLatin1Char('0'));
        QString dest = m_collection->getStorageDir(getCurrentId(), "turntable").absoluteFilePath(filename);
        QFile::remove(dest);
        QFile::copy(":/default-model/suzanne/"+filename, dest);
        QFile::setPermissions(dest, QFileDevice::ReadOwner|QFileDevice::WriteOwner);
        ui->turntable->setPicture(i-1, filename);
        this->ui->workBar->setValue(i);
    }
    ui->turntable->setView(0);
    ui->turntable->fitInView();
    ui->angleBox->setSingleStep(ui->turntable->getAngleStep());
    updateSaveButton();
}

void MainWindow::on_manButton_clicked()
{
    ui->turntable->setNumber(36);
    this->startWork(tr("Loading views"), 36);
    for(int i=1; i<=36; ++i)
    {
        QString filename = QString("%1.jpg").arg(QString::number(i), 3, QLatin1Char('0'));
        QString dest = m_collection->getStorageDir(getCurrentId(), "turntable").absoluteFilePath(filename);
        QFile::remove(dest);
        QFile::copy(":/default-model/man/"+filename, dest);
        QFile::setPermissions(dest, QFileDevice::ReadOwner|QFileDevice::WriteOwner);
        ui->turntable->setPicture(i-1, filename);
        this->ui->workBar->setValue(i);
    }
    ui->turntable->setView(0);
    ui->turntable->fitInView();
    ui->angleBox->setSingleStep(ui->turntable->getAngleStep());
    updateSaveButton();
}

void MainWindow::on_actionNew_Collection_triggered()
{
    QString path = "";
    while(path.isEmpty())
        path = QFileDialog::getSaveFileName(this, tr("New collection"), QDir::home().absolutePath());
    QFile::remove(path);
    QFile::copy(":/default-db/default.db",path);
    QFile::setPermissions(path, QFileDevice::ReadOwner|QFileDevice::WriteOwner);
    loadCollection(path);
    QDir src(":/default-model/man");
    QStringList files = src.entryList(QDir::Files, QDir::Name);
    QDir dest = m_collection->getStorageDir(1, "turntable");
    foreach(QString file, files) {
        QFile::copy(src.absoluteFilePath(file), dest.absoluteFilePath(file));
        QFile::setPermissions(dest.absoluteFilePath(file), QFileDevice::ReadOwner|QFileDevice::WriteOwner);
    }

    QDir src2(":/default-model/suzanne");
    files = src2.entryList(QDir::Files, QDir::Name);
    dest = m_collection->getStorageDir(2, "turntable");
    foreach(QString file, files) {
        QFile::copy(src2.absoluteFilePath(file), dest.absoluteFilePath(file));
        QFile::setPermissions(dest.absoluteFilePath(file), QFileDevice::ReadOwner|QFileDevice::WriteOwner);
    }
    m_mapper.toFirst();
}

void MainWindow::on_actionOpen_Collection_triggered()
{
    loadCollection(QFileDialog::getOpenFileName(this, tr("Open collection"), QDir::home().absolutePath()));
}

void MainWindow::on_removeButton_clicked()
{
    foreach(QListWidgetItem *item, ui->collectionTable2->selectedItems()) {
        m_collection->deleteCostume(item->data(Qt::UserRole).toInt());
        ui->collectionTable2->setItemHidden(item, true);
    }
}

void MainWindow::on_saveButton_clicked()
{
    m_collection->submit();
}

int MainWindow::getCurrentCostumeId() const
{
    return m_currentCostumeId;
}

void MainWindow::setCurrentCostumeId(int value)
{
    if(value != m_currentCostumeId) {
        m_currentCostumeId = value;
        ui->turntable->loadDirs(m_collection->getAllDirs(m_currentCostumeId,"turntable"));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (saveDialog()) {
        m_collection->cleanUp();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::on_massCaptureButton_clicked()
{
    if(m_camera != 0) {
        MassCapture *synchroniser = new MassCapture(this);
        m_progressDialog->setLabelText(tr("Mass capture..."));
        m_progressDialog->setCancelButtonText(tr("Abort Capture"));
        m_progressDialog->setRange(0, ui->numberOfPhotosSpin->value()-1);
        m_progressDialog->setValue(0);
        //m_progressDialog->setModal(Qt::WindowModal);

        ui->turntable->setNumber(ui->numberOfPhotosSpin->value());

        connect(synchroniser, SIGNAL(done()), synchroniser, SLOT(deleteLater()));
        connect(synchroniser, SIGNAL(problem(MassCapture::Problem)), this, SLOT(onMassCaptureProblem(MassCapture::Problem)));

        connect(synchroniser, &MassCapture::progress, [=](int index,QString path){
            QString filename = convertRaw(path);
            ui->turntable->setPictureAndView(index, filename);
        });

        connect(synchroniser, SIGNAL(progress(int,QString)), m_progressDialog, SLOT(setValue(int)));
        connect(synchroniser, SIGNAL(destroyed()), ui->workBar, SLOT(reset()));
        connect(synchroniser, SIGNAL(destroyed()), m_progressDialog, SLOT(reset()));
        connect(m_progressDialog, SIGNAL(canceled()), synchroniser, SLOT(deleteLater()));
        synchroniser->massCapture(m_camera, m_arduinoCommunication, m_collection, getCurrentId(), ui->numberOfPhotosSpin->value());
    } else {
        this->displayError(tr("No camera connected"), "");
    }
}

void MainWindow::on_actionSettings_triggered()
{
    m_settingsForm->show();
}

void MainWindow::on_rotateToViewButton_clicked()
{
    m_arduinoCommunication->rotationMessage(ui->viewDial->value())->launch();
}
