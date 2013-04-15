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
public:
    explicit QTurntable(QWidget *parent = 0);
    virtual void wheelEvent(QWheelEvent *event);
    int getZoomStep();
    int getAngleStep();
    
signals:
    void zoomChanged(int zoom);
    void angleChanged(int angle);
    
public slots:
    virtual void addPixmap (const QPixmap &);
    virtual void setNumber(int n);
    virtual void setPixmap(int index, QPixmap &pixmap);
    virtual void setPixmap(int index, QString path);
    virtual void setCurrentPixmap(QPixmap & pixmap);
    virtual void setCurrentPixmap(QString path);
    virtual void setView(int view);
    virtual void setAngle(int angle);
    virtual void fitInView();
    virtual void resetScale();
    virtual void setZoom(int zoom);
    virtual void zoom(int factor);
    

private:
    int computeZoom();

    QVector<QPixmap> m_pixmaps;
    int m_current;
    int m_zoom;
    int m_min_zoom,m_max_zoom;
    double m_zoom_step;
    QGraphicsPixmapItem *m_current_pixmap;
};

#endif // QTURNTABLE_H
