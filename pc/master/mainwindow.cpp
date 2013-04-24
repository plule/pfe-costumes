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
    ui->setupUi(this);
    ui->turntable->resize(800,600);
    ui->centralwidget->adjustSize();
    // hack because tr() won't work out of the class
    Collection::InitDefaultInfos();

    // Load last collection
    if(settings.value("collection").type() == QVariant::String) {
        loadCollection(settings.value("collection").toString());
        populateList(collection->getCollectionModel(), ui->collectionTable2, -1);
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

        // Configuration of the mapper between costume info widget and database model
        mapper.setModel(model);
        mapper.clearMapping();
        connect(&mapper, SIGNAL(currentIndexChanged(int)), this, SLOT(completeLoadCostume(int)));
        connect(&mapper, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshList(int)));

        // Creation of the widgets that contains costumes info
        clearLayout(ui->infoLayout, true);
        QList<QPair<Costume_info, QString> > collectionInfos = collection->sortedValidInformations();
        for(int i=0; i < collectionInfos.length(); i++) {
            QString key = collectionInfos.at(i).second;
            Costume_info info = collectionInfos.at(i).first;
            QWidget *widget = 0;
            if(!info.external) {
                if(info.type == ShortString)
                    widget = new QLineEdit(this);
                else if(info.type == Number || info.type == PK) {
                    widget = new QSpinBox(this);
                    ((QSpinBox*)widget)->setMaximum(9999);
                    ((QSpinBox*)widget)->setMinimum(-9999);
                } else if(info.type == LongString)
                    widget = new QPlainTextEdit(this);
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
                    if(info.visible)
                        ui->infoLayout->addRow(info.name, widget);
                    else
                        widget->setVisible(false);
                    mapper.addMapping(widget, model->record().indexOf(key));
                }
            }
        }
        // Externals (not in the form)
        mapper.addMapping(ui->turntable, model->record().indexOf("turntable"));

        mapper.toFirst();
    }
}

int MainWindow::getCurrentId()
{
    return ((QSpinBox *)mapper.mappedWidgetAt(collection->getIndexOf("id")))->value();
}

void MainWindow::populateList(QSqlTableModel *model, QListWidget *widget, int loaded)
{
    widget->clear();
    model->select();
    int nameRow = model->fieldIndex("character");
    for(int i=0; i<model->rowCount(); i++) {
        QSqlRecord r = model->record(i);
        QListWidgetItem *item = new QListWidgetItem(r.value(nameRow).toString());
        widget->insertItem(i,item);
    }
    if(loaded >= 0)
        widget->item(loaded)->setTextColor(Qt::red);
}

void MainWindow::refreshList(int loaded)
{
    populateList(collection->getCollectionModel(), ui->collectionTable2, loaded);
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

void MainWindow::completeLoadCostume(int index)
{
    int id = getCurrentId();
    ui->turntable->setRelativePath(collection->getStorageDir(id, "turntable"));
    ui->turntable->loadPreparedPath();
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
        QString filename = QString("%1").arg(ui->turntable->getView());
        QString path = collection->getStorageDir(getCurrentId(), "turntable").absoluteFilePath(filename);
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
        QString filename = QString("%1.jpg").arg(ui->turntable->getNumber());
        QString path = collection->getStorageDir(getCurrentId(), "turntable").absoluteFilePath(filename);
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
    collection->getCollectionModel()->insertRecord(-1, QSqlRecord());
    collection->getCollectionModel()->select();
    mapper.toLast();
}

void MainWindow::on_suzanneButton_pressed()
{
    ui->turntable->setNumber(36);
    this->startWork(tr("Loading views"), 36);
    for(int i=1; i<=36; ++i)
    {
        QString dest = collection->getStorageDir(getCurrentId(), "turntable").absoluteFilePath(QString("%1.jpg").arg(i));
        QFile::copy(QString(":/default-model/suzanne/%1.jpg").arg(i), dest);
        ui->turntable->setPicture(i-1, QString("%1.jpg").arg(i));
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
        QString dest = collection->getStorageDir(getCurrentId(), "turntable").absoluteFilePath(QString("%1.jpg").arg(i));
        QFile::copy(QString(":/default-model/man/%1.jpg").arg(i), dest);
        ui->turntable->setPicture(i-1, QString("%1.jpg").arg(i));
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

void MainWindow::on_loadButton_clicked()
{
    int row = ui->collectionTable2->currentRow();
    mapper.setCurrentIndex(row);
    ui->turntable->loadPreparedPath();
    refreshList(row);
    ui->collectionTable2->setCurrentRow(row);
}

void MainWindow::on_removeButton_clicked()
{
    QItemSelection selection(ui->collectionTable2->selectionModel()->selection() );

    QList<int> rows;
    foreach( const QModelIndex & index, selection.indexes() ) {
        rows.append( index.row() );
    }

    qSort( rows );

    int prev = -1;
    for( int i = rows.count() - 1; i >= 0; i -= 1 ) {
        int current = rows[i];
        if( current != prev ) {
            collection->getCollectionModel()->removeRows( current, 1 );
            prev = current;
        }
    }
    collection->getCollectionModel()->select();
}

void MainWindow::on_saveButton_clicked()
{
    if(!mapper.submit())
        qDebug() << ((QSqlQueryModel)mapper.model()).lastError();
}
