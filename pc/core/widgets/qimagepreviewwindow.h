#ifndef QIMAGEPREVIEWWINDOW_H
#define QIMAGEPREVIEWWINDOW_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

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
    
private:
    Ui::QImagePreviewWindow *ui;
    QGraphicsScene m_scene;
    QGraphicsPixmapItem *m_pixmapItem;
};

#endif // QIMAGEPREVIEWWINDOW_H
