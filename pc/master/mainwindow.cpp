#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Logger that show what goes through the slots
    logger = new SlotLog();

    // Handle cameras (listing, taking photos, etc...)
    handler = new QPhoto::CameraHandler();
    connect(handler, SIGNAL(message(QString)), this, SLOT(updateStatusBar(QString)));
    connect(handler, SIGNAL(refreshed()), this, SLOT(refresh()));
    handler->init();
    doCamerasConnections();

    // Ui init and tweaks
    QIcon::setThemeName("elementary-xfce"); // TODO fix this.
    ui->setupUi(this);
    ui->turntable->resize(800,600);
    ui->centralwidget->adjustSize();

    // hack because tr() won't work out of the class
    Collection::InitDefaultInfos();

    // Load last collection
    if(settings.value("collection").type() == QVariant::String && QFile::exists(settings.value("collection").toString())) {
        loadCollection(settings.value("collection").toString());
    }

    captureAction = Ignore;
}

// Utility to remove all widgets from a layout
static void clearLayout(QLayout* layout, bool deleteWidgets = true)
{
    while (QLayoutItem* item = layout->takeAt(0))
    {
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
        QList<QPair<Costume_info, QString> > collectionInfos = collection->sortedValidInformations();
        for(int i=0; i < collectionInfos.length(); i++) {
            QString key = collectionInfos.at(i).second;
            Costume_info info = collectionInfos.at(i).first;
            QWidget *widget = 0;
            if(info.type == ShortString) {
                QLineEdit *lineEdit = new QLineEdit(this);
                lineEdit->setCompleter(new QCompleter(*collection->getExistings(key), lineEdit));
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

MainWindow::~MainWindow()
{
    delete ui;
    delete handler;
}

void MainWindow::refresh()
{
    qDebug() << handler->getNbCameras();
}

void MainWindow::startWork(QString work, int target)
{
    this->ui->workBar->setFormat(sender()->objectName() + " : " + work + " (%p%)");
    this->ui->workBar->setMaximum(target);
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
    // TODO : safe
    QString filename = QFileInfo(path).fileName();
    switch(captureAction)
    {
    case Ignore:
        qWarning() << "Got a photo for unknown reason";
        break;
    case Append:
        captureAction = Ignore;
        ui->turntable->addPicture(filename);
        break;
    case Replace:
        captureAction = Ignore;
        ui->turntable->setCurrentPicture(filename);
        break;
    default:
        captureAction = Ignore;
        qWarning() << "Got a photo for unknown reason";
        break;
    }
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
    QPhoto::QCamera **cameras;
    captureAction = Replace;
    if(handler->getCameras(&cameras) >= 1)
    {
        QPhoto::QCamera *camera = cameras[0];
        QString filename = QString("%1").arg(QString::number(ui->turntable->getView()), 3, QLatin1Char('0'));
        QString path = collection->getStorageDir(getCurrentId(), "turntable").absoluteFilePath(filename);
        qDebug() << path;
        QMetaObject::invokeMethod(camera, "captureToFile", Qt::QueuedConnection, Q_ARG(QString, path));
    } else {
        this->displayError(tr("No camera connected"));
    }
}


void MainWindow::on_appendCaptureButton_clicked()
{
    QPhoto::QCamera **cameras;
    captureAction = Append;
    if(handler->getCameras(&cameras) >= 1)
    {
        QPhoto::QCamera *camera = cameras[0];
        QString filename = QString("%1.jpg").arg(QString::number(ui->turntable->getNumber()), 3, QLatin1Char('0'));
        QString path = collection->getStorageDir(getCurrentId(), "turntable").absoluteFilePath(filename);
        qDebug() << path;
        QMetaObject::invokeMethod(camera, "captureToFile", Qt::QueuedConnection, Q_ARG(QString, path));
    } else {
        this->displayError(tr("No camera connected"));
    }
}

void MainWindow::on_refreshButton_clicked()
{
    handler->refreshCameraList();
    doCamerasConnections();
}

void MainWindow::doCamerasConnections()
{
    QPhoto::QCamera **cameras;
    int nConnections = handler->getCameras(&cameras);
    for(int i=0; i < nConnections; i++) {
        connect(cameras[i], SIGNAL(error(QString)), logger, SLOT(error(QString)));
        connect(cameras[i], SIGNAL(idle()), logger, SLOT(idle()));
        connect(cameras[i], SIGNAL(status(QString)), logger, SLOT(message(QString)));
        connect(cameras[i], SIGNAL(message(QString)), logger, SLOT(message(QString)));
        connect(cameras[i], SIGNAL(progress_update(int)), logger, SLOT(progress_update(int)));
        connect(cameras[i], SIGNAL(progress_start(QString, int)), logger, SLOT(progress_start(QString, int)));

        connect(cameras[i], SIGNAL(progress_start(QString,int)), this, SLOT(startWork(QString,int)));
        connect(cameras[i], SIGNAL(progress_update(int)), this->ui->workBar, SLOT(setValue(int)));
        connect(cameras[i], SIGNAL(captured(QString)), this, SLOT(handleNewPicture(QString)));

        connect(cameras[i], SIGNAL(error(QString)), this->statusBar(), SLOT(showMessage(QString)));
        connect(cameras[i], SIGNAL(operation_failed(QString)), this, SLOT(displayError(QString)));
        connect(cameras[i]->getWatchdog(), SIGNAL(timeout()), this, SLOT(timeout()));
    }
}

void MainWindow::on_newCostume_clicked()
{
    int newId = collection->newCostume();
    QListWidgetItem *item = new QListWidgetItem(tr("New Costume"), ui->collectionTable2);
    item->setData(Qt::UserRole, newId);
    item->setIcon(QIcon::fromTheme("x-office-document"));
    ui->collectionTable2->setDirty(item, true);
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
}

void MainWindow::on_actionNew_Collection_triggered()
{
    loadCollection(QFileDialog::getSaveFileName(this, tr("New collection"), QDir::home().absolutePath()));
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
    currentCostumeId = value;
    ui->turntable->loadDir(collection->getStorageDir(value,"turntable"));
}
