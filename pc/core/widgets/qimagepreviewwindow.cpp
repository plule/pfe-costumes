#include "qimagepreviewwindow.h"
#include "ui_qimagepreviewwindow.h"

QImagePreviewWindow::QImagePreviewWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QImagePreviewWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(&m_scene);
    m_pixmapItem = m_scene.addPixmap(QPixmap());
    this->resize(800,600);
}

QImagePreviewWindow::~QImagePreviewWindow()
{
    delete ui;
}

bool QImagePreviewWindow::setPicture(QString path)
{
    QPixmap pix(path);
    if(path.isNull())
        return false;
    m_pixmapItem->setPixmap(pix);
    ui->graphicsView->fitInView(m_pixmapItem, Qt::KeepAspectRatio);
    return true;
}

void QImagePreviewWindow::resizeEvent(QResizeEvent *e)
{
    (void)e;
    ui->graphicsView->fitInView(m_pixmapItem, Qt::KeepAspectRatio);
}

void QImagePreviewWindow::on_closeButton_clicked()
{
    this->hide();
}

void QImagePreviewWindow::on_recaptureButton_clicked()
{
    emit recaptureRequired();
}
