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

    QDir getRelativePath() const;
    void setRelativePath(const QDir &value);

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
    virtual void loadDir(QDir dir);

private:
    int computeZoom();
    QString getPathOf(QString filename);

    QDir relativePath;
    QVector<QPair<QString,QPixmap> > m_pixmaps;
    int m_current;
    int m_zoom;
    int m_min_zoom,m_max_zoom;
    double m_zoom_step;
    QGraphicsPixmapItem *m_current_pixmap;
};

#endif // QTURNTABLE_H
