#ifndef QTURNTABLE_H
#define QTURNTABLE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QWheelEvent>
#include "math.h"

class QTurntable : public QGraphicsView
{
    Q_OBJECT

    Q_PROPERTY(QString paths READ getPaths WRITE loadPaths USER true)
public:
    explicit QTurntable(QWidget *parent = 0);
    virtual void wheelEvent(QWheelEvent *event);
    int getZoomStep();
    int getAngleStep();
    QString getPaths();
    void loadPaths(QString paths);
    
signals:
    void zoomChanged(int zoom);
    void angleChanged(int angle);
    
public slots:
    virtual void setNumber(int n);
    virtual void addPicture(QString path);
    virtual void setPicture(int index, QString path);
    virtual void setCurrentPicture(QString path);
    virtual void setView(int view);
    virtual void setAngle(int angle);
    virtual void fitInView();
    virtual void resetScale();
    virtual void setZoom(int zoom);
    virtual void zoom(int factor);

    virtual void setPicture(int index, QPixmap &pixmap);
    virtual void setCurrentPicture(QPixmap & pixmap);
    virtual void addPicture (const QPixmap &);


private:
    int computeZoom();

    QVector<QPixmap> m_pixmaps;
    QString paths;
    int m_current;
    int m_zoom;
    int m_min_zoom,m_max_zoom;
    double m_zoom_step;
    QGraphicsPixmapItem *m_current_pixmap;
};

#endif // QTURNTABLE_H
