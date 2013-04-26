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
#include "math.h"

class QTurntable : public QGraphicsView
{
    Q_OBJECT

public:
    explicit QTurntable(QWidget *parent = 0);
    virtual void wheelEvent(QWheelEvent *event);
    int getZoomStep();
    int getAngleStep();
    int getNumber();
    int getView();

signals:
    void zoomChanged(int zoom);
    void angleChanged(int angle);
    void loadStart(QString task, int target);
    void loadUpdate(int current);
    void loadComplete();
    
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
    virtual void loadDirs(QList<QDir> dirs, bool force=false);

private:
    int computeZoom();
    QString getPathOf(QString filename);

    QList<QDir> relativePaths;
    QVector<QPair<QString,QPixmap> > m_pixmaps;
    int m_current;
    int m_zoom;
    int m_min_zoom,m_max_zoom;
    double m_zoom_step;
    QGraphicsPixmapItem *m_current_pixmap;
};

#endif // QTURNTABLE_H
