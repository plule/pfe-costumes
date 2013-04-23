#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    logger = new SlotLog();
    handler = new QPhoto::CameraHandler();
    costume = new Costume();
    ui->setupUi(this);
    ui->turntable->resize(800,600);
    ui->centralwidget->adjustSize();

    Costume::InitDefaultInfos(); // hack because tr() won't work out of the class
    initInfoLayout(ui->infoLayout, Costume::valid_informations);

    connect(handler, SIGNAL(message(QString)), this, SLOT(updateStatusBar(QString)));
    connect(handler, SIGNAL(refreshed()), this, SLOT(refresh()));

    handler->init();
    doConnections();
    qDebug() << QIcon::themeName();
    ui->turntable->setNumber(36);
    if(settings.value("collection").type() == QVariant::String)
        loadCollection(settings.value("collection").toString());
}

void MainWindow::loadCollection(QString path)
{
    settings.setValue("collection", path);
    if(collection.init(path)) {
        collection.createCollectionTable();
        QSqlTableModel *model = collection.getCollectionModel();

        model->setTable("collection");
        model->select();
        ui->collectionTable->setModel(model);
        mapper.setModel(model);
        ui->collectionTable->setModelColumn(collection.getIndexOf("generated_name"));
        connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), ui->collectionTable, SLOT(update(QModelIndex)));
        connect(ui->collectionTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), &mapper, SLOT(setCurrentModelIndex(QModelIndex)));
        mapper.clearMapping();
        foreach(QString key, Costume::valid_informations.keys())
            mapper.addMapping(infoWidgets.value(key), model->record().indexOf(key));
        mapper.toFirst();
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

void MainWindow::timeout()
{
    statusBar()->showMessage(tr("Lost camera."));
    this->displayError(tr("Lost camera... You should try to disconnect and reconnect it."));
}

void MainWindow::on_captureButton_clicked()
{
    QPhoto::QCamera **cameras;
    if(handler->getCameras(&cameras) >= 1)
    {
        QPhoto::QCamera *camera = cameras[0];
        QString path = QDir::temp().absoluteFilePath("test.jpg");
        QMetaObject::invokeMethod(camera, "captureToFile", Qt::QueuedConnection, Q_ARG(QString, path));
    } else {
        this->displayError(tr("No camera connected"));
    }
}

void MainWindow::on_refreshButton_clicked()
{
    handler->refreshCameraList();
    doConnections();
}

void MainWindow::doConnections()
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
        connect(cameras[i], SIGNAL(captured(QString)), this->ui->turntable, SLOT(setCurrentPixmap(QString)));

        connect(cameras[i], SIGNAL(error(QString)), this->statusBar(), SLOT(showMessage(QString)));
        connect(cameras[i], SIGNAL(operation_failed(QString)), this, SLOT(displayError(QString)));
        connect(cameras[i]->getWatchdog(), SIGNAL(timeout()), this, SLOT(timeout()));
    }
}

typedef QPair<Costume_info,QString> info_for_sort;

void MainWindow::initInfoLayout(QFormLayout *layout, QMap<QString, Costume_info> valid_informations)
{
    QList<QPair<Costume_info, QString> > orderedInfos;

    foreach(QString key, valid_informations.keys())
        orderedInfos << QPair<Costume_info, QString>(valid_informations.value(key), key);

    qSort(orderedInfos);

    foreach(info_for_sort pair, orderedInfos)
    {
        Costume_info info = pair.first;
        QString key = pair.second;
        QWidget *widget = 0;
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

            layout->addRow(info.name, button);
            infoWidgets.insert(key, dialog);
        } else
            qWarning() << QString("Unknown field type for field ") + info.name;

        if(widget != 0) {
            infoWidgets.insert(key, widget);
            layout->addRow(info.name, widget);
        }
    }
}

void MainWindow::loadCostume(Costume *costume)
{
    foreach(QString key, costume->getInfos().keys()) {
        Costume_info info = Costume::valid_informations.value(key);
        QVariant value = costume->getInfo(key);
        if(info.type == ShortString) {
            QLineEdit *widget = (QLineEdit*)infoWidgets.value(key);
            widget->setText(value.toString());
        } else if(info.type == LongString) {
            QPlainTextEdit *widget = (QPlainTextEdit*)infoWidgets.value(key);
            widget->setPlainText(value.toString());
        } else if(info.type == Number) {
            QSpinBox *widget = (QSpinBox*)infoWidgets.value(key);
            widget->setValue(value.toInt());
        }
    }
}

void MainWindow::on_newCostume_clicked()
{
    collection.getCollectionModel()->insertRecord(-1, QSqlRecord());
    collection.getCollectionModel()->select();
    mapper.toLast();
}

void MainWindow::on_suzanneButton_pressed()
{
    this->startWork(tr("Loading views"), 36);
    for(int i=1; i<=36; ++i)
    {
        ui->turntable->setPixmap(i-1, QString(":/default-model/%1.jpg").arg(i));
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
    QModelIndex index = ui->collectionTable->selectionModel()->selectedIndexes().first();
    costume = collection.loadCostume(collection.getCollectionModel()->record(index.row()).value("id").toInt());
    mapper.setCurrentIndex(index.row());
}

void MainWindow::on_removeButton_clicked()
{
    QItemSelection selection(ui->collectionTable->selectionModel()->selection() );

    QList<int> rows;
    foreach( const QModelIndex & index, selection.indexes() ) {
        rows.append( index.row() );
    }

    qSort( rows );

    int prev = -1;
    for( int i = rows.count() - 1; i >= 0; i -= 1 ) {
        int current = rows[i];
        if( current != prev ) {
            collection.getCollectionModel()->removeRows( current, 1 );
            prev = current;
        }
    }
    collection.getCollectionModel()->select();
}

void MainWindow::on_saveButton_clicked()
{
    if(!mapper.submit())
        qDebug() << ((QSqlQueryModel)mapper.model()).lastError();
}
