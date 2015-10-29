#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(bool noedit, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_collection = 0;
    m_camera = 0;
    m_progressDialog = new QProgressDialog();
    m_noedit = noedit;
    // Ui init and tweaks
    ui->setupUi(this);

    // Additional more complexe connections
    ui->viewDial->setMinimum(0);
    connect(ui->turntable, &QTurntable::viewChanged, [=](int view){
        if(ui->turntable->getNumber() > 0)
            ui->angleBox->setValue(view*360/ui->turntable->getNumber());
    });
    connect(ui->turntable, &QTurntable::numberChanged, [=](int number){
        if(number > 0) {
            ui->angleBox->setSingleStep(360/number);
            ui->angleBox->setValue(ui->turntable->getView()*360/number);
            ui->viewDial->setMaximum(number-1);
        }
    });
    void (QSpinBox:: *spinboxSignal)(int) = &QSpinBox::valueChanged;
    connect(ui->angleBox, spinboxSignal, [=](int angle){
        ui->turntable->setView(ui->turntable->getNumber()*angle/360);
    });

    // Camera handler and arduino communication modules
    m_handler = new QPhoto::CameraHandler();
    m_arduinoCommunication = new ArduinoCommunication(this);

    // Handle cameras (listing, taking photos, etc...)
    connect(m_handler, SIGNAL(message(QString)), this, SLOT(updateStatusBar(QString)));
    m_handler->init();

    // Settings window
    m_settingsForm = new SettingsForm(m_handler, m_arduinoCommunication, this);
    connect(m_settingsForm, SIGNAL(cameraChanged(QPhoto::QCamera*)), this, SLOT(setCamera(QPhoto::QCamera*)));
    setCamera(m_settingsForm->getCamera());

    // Image Preview Window
    m_imagePreview = new QImagePreviewWindow(this);
    connect(m_imagePreview, &QImagePreviewWindow::recaptureRequired, this, &MainWindow::on_capturePreviewButton_clicked);

    // Arduino comm configuration and ui init
    m_arduinoCommunication->setPort(m_settingsForm->getXbeePort());

    // Monitor background color changes
    m_imagePreview->setBackgroundColor(m_settings.value(S_BACKGROUNDCOLOR).value<QColor>());
    ui->turntable->setBackgroundColor(m_settings.value(S_BACKGROUNDCOLOR).value<QColor>());
    connect(m_settingsForm, &SettingsForm::backgroundColorChanged, m_imagePreview, &QImagePreviewWindow::setBackgroundColor);
    connect(m_settingsForm, &SettingsForm::backgroundColorChanged, ui->turntable, &QTurntable::setBackgroundColor);

    // Handle messages from the arduino

    // Update slider's position according to arduino's messages
    connect(m_arduinoCommunication, &ArduinoCommunication::motorDistanceChanged, [=](QString arduino, int motor, int distance, bool calibrated){
        if(m_adjustmentGroups.contains(arduino)) {
            QWidget *group = m_adjustmentGroups.value(arduino);
            for(int i=0; i<group->layout()->count(); ++i) {
                QEllipseSlider *ellipseSlider = (QEllipseSlider *)group->layout()->itemAt(i)->widget();
                if(ellipseSlider->property("side_motor").toInt() == motor) {
                    ellipseSlider->setSideSliderEnabled(calibrated);
                    ellipseSlider->setSideMotorValue(distance);
                    return;
                }
                if(ellipseSlider->property("front_motor").toInt() == motor) {
                    ellipseSlider->setFrontSliderEnabled(calibrated);
                    ellipseSlider->setFrontMotorValue(distance);
                    return;
                }
            }
            qWarning() << tr("Distance motor message from known model for unknown motor");
        } else {
            qWarning() << tr("Distance motor message from unknown model");
        }
    });

    // Update bounds' values according to arduino's messages
    connect(m_arduinoCommunication, &ArduinoCommunication::motorBoundsChanged, [=](QString arduino, int motor, int umin, int umax){
        if(m_adjustmentGroups.contains(arduino)) {
            QWidget *group = m_adjustmentGroups.value(arduino);
            for(int i=0; i<group->layout()->count(); ++i) {
                QEllipseSlider *ellipseSlider = (QEllipseSlider *)group->layout()->itemAt(i)->widget();
                if(ellipseSlider->property("side_motor").toInt() == motor) {
                    ellipseSlider->setSideMotorBounds(umin, umax);
                    return;
                }
                if(ellipseSlider->property("front_motor").toInt() == motor) {
                    ellipseSlider->setFrontMotorBounds(umin, umax);
                    return;
                }
            }
            qWarning() << tr("Distance motor message from known model for unknown motor");
        } else {
            qWarning() << tr("Distance motor message from unknown model");
        }
    });

    // When a new arduino is detected, a group of QEllipseSlider is created (one per module)
    connect(m_arduinoCommunication, &ArduinoCommunication::arduinoDetected, [=](QString arduino,QString name, ARD_ROLE role){
        if(role == ROLE_MORPHOLOGY) {
            QWidget *adjGroup = createAdjustmentGroup(arduino);
            if(ui->ardListCombo->itemData(ui->ardListCombo->currentIndex()).toString() != arduino) // do not hide if it's the current arduino
                adjGroup->setVisible(false);
            m_adjustmentGroups.insert(arduino,adjGroup);
            ui->adjustmentScroll->layout()->addWidget(adjGroup);
            m_arduinoCommunication->getMotorsPositionMessage(arduino)->launch(); // Request current motors' positions
            m_arduinoCommunication->getMotorsBoundMessage(arduino)->launch(); // Request current motors' bounds
            statusBar()->showMessage(tr("Model %1 connected").arg(name));
        }
        ui->turntableButton->setDisabled(m_arduinoCommunication->listTurntables().isEmpty());
    });

    // When an arduino is disconnected, its QEllipseSlider group is distroyed
    connect(m_arduinoCommunication, &ArduinoCommunication::arduinoLost, [=](QString arduino,QString name) {
        if(m_adjustmentGroups.contains(arduino)) {
            m_adjustmentGroups.value(arduino)->deleteLater();
            m_adjustmentGroups.remove(arduino);
            statusBar()->showMessage(tr("Model %1 disconnected").arg(name));
        }
        ui->turntableButton->setDisabled(m_arduinoCommunication->listTurntables().isEmpty());
    });


    // When the user changes the selected arduino, its qellipseslider group is set visible
    void (QComboBox:: *signal)(int) = &QComboBox::currentIndexChanged; // bad qt5 signal hack
    connect(ui->ardListCombo, signal, [=](int index){
        //int index = ui->ardListCombo->currentIndex();
        QString arduino = ui->ardListCombo->itemData(index, Qt::UserRole).toString();
        if(index >= 0 && m_adjustmentGroups.contains(arduino)) {
            QWidget *newGroup = m_adjustmentGroups.value(arduino);
            foreach(QWidget *group, m_adjustmentGroups) {
                if(newGroup != group)
                    group->setVisible(false);
                else
                    group->setVisible(true);
            }
        }
    });

    // The combo displaying the arduinos shares its model with the object communicating with the arduinos
    ui->ardListCombo->setModel(m_arduinoCommunication->model());

    m_arduinoCommunication->helloMessage()->launch();
    connect(m_settingsForm, SIGNAL(xbeePortChanged(QString)), m_arduinoCommunication, SLOT(setPort(QString)));

    // Load last collection
    if(m_settings.value(S_COLLECTION).type() == QVariant::String && QFile::exists(m_settings.value("collection").toString()))
        loadCollection(m_settings.value(S_COLLECTION).toString());
    else
        on_actionNew_Collection_triggered();

    // Disable controls if noedit mode
    if(m_noedit) {
        ui->newCostume->setVisible(false);
        ui->saveButton->setVisible(false);
        ui->removeButton->setVisible(false);
        ui->captureBox->setVisible(false);
        ui->turntableBox->setVisible(false);
        ui->tab->removeTab(0);
        ui->infoTab->setEnabled(false);
        ui->actionNew_Collection->setVisible(false);
        ui->actionSettings->setVisible(false);
    }

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
    m_settings.setValue(S_COLLECTION, path);
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
    /*if(m_rawHandler.IsRaw(path)) {

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
    }*/
    return filename;
}

void MainWindow::updateSaveButton()
{
    if(!m_noedit)
        ui->saveButton->setEnabled(m_collection->isDirty());
    else
        ui->saveButton->setEnabled(false);
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

void MainWindow::setCamera(QPhoto::QCamera *camera)
{
    if(camera != m_camera) {
        qDebug() << "set camera " << camera;
        this->m_camera = camera;
        if(camera != 0) {
            connect(camera, SIGNAL(progress_start(QString,int)), this, SLOT(startWork(QString,int)));
            connect(camera, SIGNAL(progress_update(int)), this->ui->workBar, SLOT(setValue(int)));
            connect(camera, SIGNAL(error(QString)), this->statusBar(), SLOT(showMessage(QString)));
            connect(camera, &QPhoto::QCamera::finished, [=](int status){
                if(status == QPhoto::QCamera::Error || status == QPhoto::QCamera::Timeout || status == QPhoto::QCamera::NotConnected) {
                    // Camera failed capture multiple time, better try to reconnect next time
                    m_camera = 0;
                }
            });
        }
    }
}

void MainWindow::onMassCaptureProblem(MassCapture::Problem problem, QString description)
{
    qDebug() << "Mass capture had a problem";
    QMessageBox errorDialog;
    if(problem == MassCapture::CameraProblem)
        errorDialog.setText(tr("The camera has encountered a problem."));
    else if(problem == MassCapture::RotationProblem)
        errorDialog.setText(tr("The rotating model has encountered a problem."));
    else
        errorDialog.setText(tr("An unknown problem occured."));
    errorDialog.setDetailedText(description);
    errorDialog.setIcon(QMessageBox::Warning);
    errorDialog.setStandardButtons(QMessageBox::Abort);
    sender()->deleteLater();
    m_progressDialog->reset();
    errorDialog.exec();
}

void MainWindow::on_newCostume_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Confirmation"), tr("Add a costume?"),QMessageBox::Yes|QMessageBox::No);
    if(reply== QMessageBox::Yes) {
        int newId = m_collection->newCostume();
        QListWidgetItem *item = new QListWidgetItem(tr("New Costume"), ui->collectionTable2);
        item->setData(Qt::UserRole, newId);
        item->setIcon(QIcon::fromTheme("x-office-document"));
        ui->collectionTable2->setDirty(item, true);
        ui->collectionTable2->selectionModel()->clear();
        ui->collectionTable2->setCurrentItem(item);
        m_mapper.toLast();
    }
}

void MainWindow::on_actionNew_Collection_triggered()
{
    QString path = "";

    while(path.isEmpty()) {
        path = QFileDialog::getSaveFileName(this, tr("New collection"), QDir::home().absolutePath());
        if(path.isEmpty() && m_collection != 0)
            return;
    }
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
    QString path = QFileDialog::getOpenFileName(this, tr("Open collection"), QDir::home().absolutePath());
    if(!path.isEmpty())
        loadCollection(path);
}

void MainWindow::on_removeButton_clicked()
{
    int nbSelected = ui->collectionTable2->selectedItems().count();
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure you want to delete these %n costume(s)?", "", nbSelected),
                                                              QMessageBox::Yes|QMessageBox::No);
    if(reply== QMessageBox::Yes) {
        foreach(QListWidgetItem *item, ui->collectionTable2->selectedItems()) {
            m_collection->deleteCostume(item->data(Qt::UserRole).toInt());
            ui->collectionTable2->setItemHidden(item, true);
        }
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
        m_progressDialog->setRange(0, m_settings.value(S_PHOTONUMBER).toInt()-1);
        m_progressDialog->setValue(0);
        //m_progressDialog->setModal(Qt::WindowModal);

        ui->turntable->setNumber(m_settings.value(S_PHOTONUMBER).toInt());

        connect(synchroniser, SIGNAL(done()), synchroniser, SLOT(deleteLater()));
        connect(synchroniser, SIGNAL(problem(MassCapture::Problem, QString)), this, SLOT(onMassCaptureProblem(MassCapture::Problem, QString)));

        connect(synchroniser, &MassCapture::progress, [=](int index,QString path){
            QString filename = convertRaw(path);
            ui->turntable->setPictureAndView(index, filename);
        });

        connect(synchroniser, SIGNAL(progress(int,QString)), m_progressDialog, SLOT(setValue(int)));
        connect(synchroniser, SIGNAL(destroyed()), ui->workBar, SLOT(reset()));
        connect(synchroniser, SIGNAL(destroyed()), m_progressDialog, SLOT(reset()));
        connect(m_progressDialog, SIGNAL(canceled()), synchroniser, SLOT(deleteLater()));
        synchroniser->massCapture(m_camera,
                                  m_arduinoCommunication,
                                  m_collection,
                                  getCurrentId(),
                                  m_settings.value(S_PHOTONUMBER).toInt(),
                                  m_settings.value(S_DELAYEDDOWNLOAD).toBool());
    } else {
        m_settingsForm->refreshCameraList();
        setCamera(m_settingsForm->getCamera());
        if(m_camera != 0) {
            on_massCaptureButton_clicked();
        } else
            this->displayError(tr("No camera connected"), "");
    }
}

void MainWindow::on_actionSettings_triggered()
{
    m_settingsForm->show();
}

QWidget *MainWindow::createAdjustmentGroup(QString arduinoId)
{
    QGroupBox *group = new QGroupBox(this);
    QVBoxLayout *layout = new QVBoxLayout(group);

    QHash<QString,QEllipseSlider*> module_sliders;
    for(int i=0; i < m_arduinoCommunication->getMotorsNumber(); i++) {
        QString name = QString(m_arduinoCommunication->getMotorName(i));
        QEllipseSlider *slider;
        if(module_sliders.contains(name))
            slider = module_sliders.value(name);
        else {
            slider = new QEllipseSlider(this);
            slider->setName(name);
            module_sliders.insert(name, slider);
            layout->addWidget(slider);
        }
        switch(m_arduinoCommunication->getMotorType(i)) {
        case FRONT_MOTOR:
            slider->setProperty("front_motor",i);
            connect(slider, &QEllipseSlider::frontMotorValueChanged, [=](int distance){
                m_arduinoCommunication->motorDistanceMessage(arduinoId, i, distance)->launch();
            });
            connect(slider, &QEllipseSlider::frontMotorLowerBoundsChanged, [=](int umin){
                m_arduinoCommunication->setClosePosition(arduinoId, i, umin)->launch();
            });
            connect(slider, &QEllipseSlider::frontMotorUpperBoundsChanged, [=](int umax){
                m_arduinoCommunication->setOpenPosition(arduinoId, i, umax)->launch();
            });
            break;
        case SIDE_MOTOR:
            slider->setProperty("side_motor",i);
            connect(slider, &QEllipseSlider::sideMotorValueChanged, [=](int distance){
                m_arduinoCommunication->motorDistanceMessage(arduinoId, i, distance)->launch();
            });
            connect(slider, &QEllipseSlider::sideMotorLowerBoundsChanged, [=](int umin){
                m_arduinoCommunication->setClosePosition(arduinoId, i, umin)->launch();
            });
            connect(slider, &QEllipseSlider::sideMotorUpperBoundsChanged, [=](int umax){
                m_arduinoCommunication->setOpenPosition(arduinoId, i, umax)->launch();
            });
            break;
        default:
            qWarning() << "Malformed interface.h";
            break;
        }

        // Handle motor's bound edition
        slider->setBoundEditEnabled(ui->enableBoundEditCheckbox->isChecked());
        connect(ui->enableBoundEditCheckbox, &QCheckBox::toggled, slider, &QEllipseSlider::setBoundEditEnabled);

        // Handle model's dimension change
        connect(m_settingsForm, &SettingsForm::modelDepthChanged, slider, &QEllipseSlider::setFrontBaseOffset);
        connect(m_settingsForm, &SettingsForm::modelWidthChanged, slider, &QEllipseSlider::setSideBaseOffset);
        slider->setBaseOffset(m_settingsForm->getModelWidth(), m_settingsForm->getModelDepth());
    }
    return group;
}

void MainWindow::on_capturePreviewButton_clicked()
{
    if(m_camera != 0) {
        m_camera->captureToFile("/tmp/preview.jpg");
        m_imagePreview->setPicture("/tmp/preview.jpg");
        m_imagePreview->show();
    } else {
        m_settingsForm->refreshCameraList();
        setCamera(m_settingsForm->getCamera());
        if(m_camera != 0) {
            on_capturePreviewButton_clicked();
        } else
            this->displayError(tr("No camera connected"), "");
    }
}

void MainWindow::on_turntableButton_toggled(bool checked)
{
    if(m_arduinoCommunication->listTurntables().size() > 0) {
        if(checked)
            m_arduinoCommunication->startRotation()->launch();
        else
            m_arduinoCommunication->stopRotation()->launch();
    }
}
