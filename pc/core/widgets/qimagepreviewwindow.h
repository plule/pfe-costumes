#ifndef QIMAGEPREVIEWWINDOW_H
#define QIMAGEPREVIEWWINDOW_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <libexif/exif-data.h>
#include "common/exifpixmap.h"

namespace Ui {
class QImagePreviewWindow;
}

class QImagePreviewWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit QImagePreviewWindow(QWidget *parent = 0);
    ~QImagePreviewWindow();

    bool setPicture(QString path);

    virtual void resizeEvent(QResizeEvent *e);


signals:
    void recaptureRequired();

private slots:
    void on_closeButton_clicked();

    void on_recaptureButton_clicked();

private:
    Ui::QImagePreviewWindow *ui;
    QGraphicsScene m_scene;
    QGraphicsPixmapItem *m_pixmapItem;
};

#endif // QIMAGEPREVIEWWINDOW_H
