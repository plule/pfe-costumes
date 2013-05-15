#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    collection = 0;
    camera = 0;
    massCaptureRunning = false;
    // Logger that show what goes through the slots
    logger = new SlotLog();

    // Ui init and tweaks
    QIcon::setThemeName("elementary-xfce"); // TODO fix this.
    ui->setupUi(this);

    // Handle cameras (listing, taking photos, etc...)
    handler = new QPhoto::CameraHandler();
    connect(handler, SIGNAL(message(QString)), this, SLOT(updateStatusBar(QString)));
    handler->init();

    // Settings window
    settingsForm = new SettingsForm(handler, this);
    connect(settingsForm, SIGNAL(cameraChanged(QPhoto::QCamera*)), this, SLOT(setCamera(QPhoto::QCamera*)));
    setCamera(settingsForm->getCamera());

    // Arduino comm
    morphology = new ArduinoCommunication(settingsForm->getXbeePort());
    connect(ui->rotationDial, SIGNAL(valueChanged(int)), morphology, SLOT(setRotation(int)));
    for(int i=0; i < morphology->getMotorsNumber(); i++) {
        QString name = QString(morphology->getMotorsNames()[i]);
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

        morphoSliders.append(slider);

        connect(slider, SIGNAL(valueChanged(int)), spin, SLOT(setValue(int)));
        connect(spin, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
        connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sendMs(int)));

        ui->adjustementForm->addRow(name, layout);
    }

    connect(morphology, SIGNAL(motorMicrosecondChanged(int,int,int)), this, SLOT(setMotorMicroSecond(int,int,int)));

    connect(ui->ardHelloButton, SIGNAL(clicked()), morphology, SLOT(sendHelloMessage()));
    connect(morphology, SIGNAL(arduinoAdded(Arduino)), this, SLOT(addDevice(Arduino)));
    connect(morphology, SIGNAL(arduinoRemoved(Arduino)), this, SLOT(removeDevice(Arduino)));
    morphology->sendHelloMessage();
    connect(settingsForm, SIGNAL(xbeePortChanged(QString)), morphology, SLOT(setPort(QString)));

    // Load last collection
    if(settings.value("collection").type() == QVariant::String && QFile::exists(settings.value("collection").toString()))
        loadCollection(settings.value("collection").toString());
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
    if(collection)
        delete collection;
    settings.setValue("collection", path);
    collection = new Collection(this, path);
    if(collection->isValid()) {
        QSqlTableModel *model = collection->getCollectionModel();

        // Update save button and titles on data change/sync
        connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
        connect(collection, SIGNAL(synchronised()), this, SLOT(updateSaveButton()));
        connect(collection, SIGNAL(synchronised()), this, SLOT(populateList())); // ensure sync. TODO useful?
        connect(collection, SIGNAL(synchronised()), ui->collectionTable2, SLOT(cleanAll()));

        // Configuration of the mapper between costume info widget and database model
        mapper.setModel(model);
        mapper.clearMapping();
        mapper.setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

        // Connection between list widget and mapper
        connect(&mapper, SIGNAL(currentIndexChanged(int)), ui->collectionTable2, SLOT(load(int)));
        connect(ui->collectionTable2, SIGNAL(loadedChanged(int)), &mapper, SLOT(setCurrentIndex(int)));

        // Creation of the widgets that contains costumes info
        clearLayout(ui->infoLayout, true);
        QList<QPair<Costume_info, QString> > collectionInfos = collection->sortedContent();
        for(int i=0; i < collectionInfos.length(); i++) {
            QString key = collectionInfos.at(i).second;
            Costume_info info = collectionInfos.at(i).first;
            QWidget *widget = 0;
            if(info.type == ShortString) {
                QLineEdit *lineEdit = new QLineEdit(this);
                if(info.autocomplete)
                    lineEdit->setCompleter(collection->getCompleter(key));
                widget = lineEdit;

            } else if(info.type == PK)
                mapper.addMapping(this, model->record().indexOf(key)); // map the pk with currentCostumeId get/set
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
                mapper.addMapping(widget, model->record().indexOf(key));
            }
        }

        populateList();
        mapper.toFirst();
    }
}

int MainWindow::getCurrentId()
{
    return currentCostumeId;
}

bool MainWindow::saveDialog()
{
    if(collection->isDirty()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Save Collection"),
                                   tr("The collection has unsaved changes.\n"
                                      "Do you want to save the modifications?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if(ret == QMessageBox::Save) {
            collection->submit();
            return true;
        } else if(ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::populateList()
{
    QSqlTableModel *model = collection->getCollectionModel();
    ui->collectionTable2->clear();
    int idRow = model->fieldIndex("id");
    for(int i=0; i<model->rowCount(); i++) {
        QSqlRecord r = model->record(i);
        QListWidgetItem *item = new QListWidgetItem(collection->getName(r));
        item->setData(Qt::UserRole, r.value(idRow));
        item->setIcon(QIcon::fromTheme("x-office-document"));
        ui->collectionTable2->insertItem(i,item);
    }
}

void MainWindow::addDevice(Arduino arduino)
{
    ui->ardListCombo->addItem(QString::number(arduino.id), arduino.id);
    if(arduino.id == getCurrentArduino())
        morphology->getMotorsPosition(arduino.id);
}

void MainWindow::removeDevice(Arduino arduino)
{
    ui->ardListCombo->removeItem(ui->ardListCombo->findData(arduino.id));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete handler;
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

void MainWindow::displayError(QString error)
{
    QErrorMessage msg;
    msg.showMessage(error);
    msg.exec();
}

void MainWindow::handleNewPicture(QString path)
{
    QString filename = QFileInfo(path).fileName();
    if(massCaptureRunning) {
        ui->turntable->addPicture(filename);
        ui->collectionTable2->setDirty(ui->collectionTable2->loadedItem(), true);
    } else {
        switch(captureActions.value(path, Ignore))
        {
        case Ignore:
            qWarning() << "Got a photo for unknown reason";
            break;
        case Append:
            ui->turntable->addPicture(filename);
            ui->collectionTable2->setDirty(ui->collectionTable2->loadedItem(), true);
            break;
        case Replace:
            ui->turntable->setCurrentPicture(filename);
            ui->collectionTable2->setDirty(ui->collectionTable2->loadedItem(), true);
            break;
        default:
            qWarning() << "Got a photo for unknown reason";
            break;
        }
        captureActions.remove(path);
    }
    updateSaveButton();
}

void MainWindow::updateSaveButton()
{
    ui->saveButton->setEnabled(collection->isDirty());
}

// Do not expect more than one change at a time.
void MainWindow::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    (void)bottomRight;
    updateSaveButton();
    QListWidgetItem *item = ui->collectionTable2->item(topLeft.row());
    if(item) {
        item->setText(collection->getName(item->data(Qt::UserRole).toInt()));
        ui->collectionTable2->setDirty(item, collection->getCollectionModel()->isDirty(topLeft));
    }
}

void MainWindow::timeout()
{
    statusBar()->showMessage(tr("Lost camera."));
    this->displayError(tr("Lost camera... You should try to disconnect and reconnect it."));
}

void MainWindow::on_captureButton_clicked()
{
    if(camera != 0) {
        QString filename = ui->turntable->getCurrentFileName();//QString("%1").arg(QString::number(ui->turntable->getView()), 3, QLatin1Char('0'));
        QString path = collection->getTempStorageDir(getCurrentId(), "turntable").absoluteFilePath(filename);
        captureActions.insert(path, Replace);
        camera->captureToFile(path);
    } else {
        this->displayError(tr("No camera connected"));
    }
}


void MainWindow::on_appendCaptureButton_clicked()
{
    if(camera != 0) {
        QString path = collection->getNewFilePath(getCurrentId(), "turntable", "jpg"); // TODO extension follow config
        captureActions.insert(path, Append);
        camera->captureToFile(path);
    } else {
        this->displayError(tr("No camera connected"));
    }
}

int MainWindow::getCurrentArduino()
{
    return ui->ardListCombo->itemData(ui->ardListCombo->currentIndex(), Qt::UserRole).toInt();
}

void MainWindow::whenMassCaptureDone(bool success)
{
    massCaptureRunning = false;
    if(!success)
        this->displayError(tr("Mass capture failed"));
}

void MainWindow::setCamera(QPhoto::QCamera *camera)
{
    //if(this->camera != 0)
        //disconnect(this->camera, 0, this, 0);
    this->camera = camera;
    if(camera != 0) {
        connect(camera, SIGNAL(error(QString)), logger, SLOT(error(QString)));
        connect(camera, SIGNAL(idle()), logger, SLOT(idle()));
        connect(camera, SIGNAL(status(QString)), logger, SLOT(message(QString)));
        connect(camera, SIGNAL(message(QString)), logger, SLOT(message(QString)));
        connect(camera, SIGNAL(progress_update(int)), logger, SLOT(progress_update(int)));
        connect(camera, SIGNAL(progress_start(QString, int)), logger, SLOT(progress_start(QString, int)));

        connect(camera, SIGNAL(progress_start(QString,int)), this, SLOT(startWork(QString,int)));
        connect(camera, SIGNAL(progress_update(int)), this->ui->workBar, SLOT(setValue(int)));
        connect(camera, SIGNAL(captured(QString)), this, SLOT(handleNewPicture(QString)));

        connect(camera, SIGNAL(error(QString)), this->statusBar(), SLOT(showMessage(QString)));
        connect(camera, SIGNAL(operation_failed(QString)), this, SLOT(displayError(QString)));
        connect(camera->getWatchdog(), SIGNAL(timeout()), this, SLOT(timeout()));
    }
}

void MainWindow::sendMs(int ms)
{
    int current = getCurrentArduino();
    int motor = sender()->property("motor").toInt();
    morphology->setMotorMicrosecond(current, motor, ms);
}

void MainWindow::setMotorMicroSecond(int arduino, int motor, int ms)
{
    if(arduino == getCurrentArduino() && motor < morphoSliders.size()) {
        morphoSliders.at(motor)->setValue(ms);
    }
}

void MainWindow::on_newCostume_clicked()
{
    int newId = collection->newCostume();
    QListWidgetItem *item = new QListWidgetItem(tr("New Costume"), ui->collectionTable2);
    item->setData(Qt::UserRole, newId);
    item->setIcon(QIcon::fromTheme("x-office-document"));
    ui->collectionTable2->setDirty(item, true);
    ui->collectionTable2->selectionModel()->clear();
    ui->collectionTable2->setCurrentItem(item);
    mapper.toLast();
}

void MainWindow::on_suzanneButton_pressed()
{
    ui->turntable->setNumber(36);
    this->startWork(tr("Loading views"), 36);
    for(int i=1; i<=36; ++i)
    {
        QString filename = QString("%1.jpg").arg(QString::number(i), 3, QLatin1Char('0'));
        QString dest = collection->getStorageDir(getCurrentId(), "turntable").absoluteFilePath(filename);
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
        QString dest = collection->getStorageDir(getCurrentId(), "turntable").absoluteFilePath(filename);
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
    QDir dest = collection->getStorageDir(1, "turntable");
    foreach(QString file, files) {
        QFile::copy(src.absoluteFilePath(file), dest.absoluteFilePath(file));
        QFile::setPermissions(dest.absoluteFilePath(file), QFileDevice::ReadOwner|QFileDevice::WriteOwner);
    }

    QDir src2(":/default-model/suzanne");
    files = src2.entryList(QDir::Files, QDir::Name);
    dest = collection->getStorageDir(2, "turntable");
    foreach(QString file, files) {
        QFile::copy(src2.absoluteFilePath(file), dest.absoluteFilePath(file));
        QFile::setPermissions(dest.absoluteFilePath(file), QFileDevice::ReadOwner|QFileDevice::WriteOwner);
    }
    mapper.toFirst();
}

void MainWindow::on_actionOpen_Collection_triggered()
{
    loadCollection(QFileDialog::getOpenFileName(this, tr("Open collection"), QDir::home().absolutePath()));
}

void MainWindow::on_removeButton_clicked()
{
    foreach(QListWidgetItem *item, ui->collectionTable2->selectedItems()) {
        collection->deleteCostume(item->data(Qt::UserRole).toInt());
        ui->collectionTable2->setItemHidden(item, true);
    }
}

void MainWindow::on_saveButton_clicked()
{
    collection->submit();
}

int MainWindow::getCurrentCostumeId() const
{
    return currentCostumeId;
}

void MainWindow::setCurrentCostumeId(int value)
{
    if(value != currentCostumeId) {
        currentCostumeId = value;
        ui->turntable->loadDirs(collection->getAllDirs(currentCostumeId,"turntable"));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (saveDialog()) {
        collection->cleanUp();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::on_massCaptureButton_clicked()
{
    if(camera != 0) {
        massCaptureRunning = true;
        MassCapture *synchroniser = new MassCapture(this);
        connect(synchroniser, SIGNAL(done(bool)), this, SLOT(whenMassCaptureDone()));
        connect(synchroniser, SIGNAL(done(bool)), synchroniser, SLOT(deleteLater()));
        synchroniser->massCapture(camera, morphology, collection, getCurrentId(), 36);
    } else {
        this->displayError(tr("No camera connected"));
    }
}

void MainWindow::on_actionSettings_triggered()
{
    settingsForm->show();
}
