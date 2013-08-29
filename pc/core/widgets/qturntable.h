#ifndef QTURNTABLE_H
#define QTURNTABLE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QWheelEvent>
#include <QPair>
#include <QStringList>
#include <QDir>
#include <QSet>
#include <libexif/exif-data.h>

#include "math.h"

/**
 * @brief The QTurntable class allow displaying a 360 view of an object.
 */
class QTurntable : public QGraphicsView
{
    Q_OBJECT

public:
    explicit QTurntable(QWidget *parent = 0);
    virtual void wheelEvent(QWheelEvent *event);
    int getZoomStep();
    int getAngleStep();

    /**
     * @brief getNumber
     * @return number of pictures
     */
    int getNumber();
    /**
     * @brief getView
     * @return index of current view
     */
    int getView();

    /**
     * @brief getCurrentFileName
     * @return name of displayed file
     */
    QString getCurrentFileName();

    /**
     * @brief enableResizePreview enable or disable resizing of the displayed picture
     * (for RAM/GPU economy)
     * @param resize
     */
    void enableResizePreview(bool resize);

    /**
     * @brief setPreviewSize set the displayed picture size
     * @param width
     * @param size
     */
    void setPreviewSize(int width, int size);

protected:
    virtual void resizeEvent(QResizeEvent *event);

    /**
     * @brief createPixmap creates a displayable and maybe rotated pixmap
     * @param path
     * @return
     */
    QPixmap createPixmap(QString path);

signals:
    /**
     * @brief zoomChanged is emitted when zoom is changed
     * @param zoom
     */
    void zoomChanged(int zoom);
    /**
     * @brief angleChanged is emitted when angle of view changed
     * @param angle
     */
    void angleChanged(int angle);
    /**
     * @brief loadStart is emitted when loading of a serie of pic started
     * @param task textual description of the task
     * @param target max value
     */
    void loadStart(QString task, int target);
    /**
     * @brief loadUpdate is emitted each time a new picture is loaded
     * @param current
     */
    void loadUpdate(int current);
    /**
     * @brief loadComplete is emitted when the pictures are all loaded
     */
    void loadComplete();
    
public slots:
    /**
     * @brief setNumber Modify the number of photo. This wipe all the photos.
     * @param n
     */
    virtual void setNumber(int n);
    /**
     * @brief addPicture append a picture
     * @param path
     */
    virtual void addPicture(QString path); // TODO delete ?
    /**
     * @brief setPicture insert a picture at an index
     * @param index between 0-number of pics
     * @param path
     */
    virtual void setPicture(int index, QString path);
    /**
     * @brief setCurrentPicture modifies current visualised picture
     * @param path
     */
    virtual void setCurrentPicture(QString path);
    /**
     * @brief setView rotate the model to the view
     * @param view
     */
    virtual void setView(int view);
    /**
     * @brief setPictureAndView modifies a picture and rotate to it
     * @param index
     * @param path
     */
    virtual void setPictureAndView(int index, QString path);
    /**
     * @brief setAngle rotate the model
     * @param angle between 0-360
     */
    virtual void setAngle(int angle);

    /**
     * @brief fitInView fits the picture in the view.
     */
    virtual void fitInView();
    /**
     * @brief resetScale do a 1:1 zoom
     */
    virtual void resetScale();
    /**
     * @brief setZoom zooms to a value
     * @param zoom (in percentage)
     */
    virtual void setZoom(int zoom);
    /**
     * @brief zoomIn increases zoom
     */
    virtual void zoomIn();
    /**
     * @brief zoomOut decreases zoom
     */
    virtual void zoomOut();
    /**
     * @brief zoom zooms by a factor (2 will zoom in, -2 will zoom out)
     * @param factor
     */
    virtual void zoom(int factor);
    /**
     * @brief loadDirs loads the content of multiple dirs. Duplicate filenames
     * will be displayed as one.
     * @param dirs
     * @param force
     */
    virtual void loadDirs(QList<QDir> dirs);

private:
    int computeZoom();
    QString getPathOf(QString filename);

    QList<QDir> relativePaths;
    QVector<QPair<QString,QGraphicsPixmapItem*> > m_pixmaps;
    QGraphicsPixmapItem *m_current_pixmap;
    bool m_resize_preview;
    QPair<int,int> m_preview_dimension;
    int m_current_angle;
    int m_zoom;
    int m_min_zoom,m_max_zoom;
    double m_zoom_step;
    bool m_fit;
    //QGraphicsPixmapItem *m_current_pixmap;
};

#endif // QTURNTABLE_H
