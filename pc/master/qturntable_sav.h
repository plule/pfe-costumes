#ifndef QTURNTABLE_H
#define QTURNTABLE_H

#include <QWidget>
#include <QAbstractSlider>
#include <QList>
#include <QPixmap>
#include <QDebug>
#include <QMouseEvent>
#include <math.h>

/*
namespace Ui {
class QTurntable;
}

class QTurntable : public QWidget
{
    Q_OBJECT
    
public:
    explicit QTurntable(QWidget *parent = 0);
    ~QTurntable();

    virtual void showController(bool show);
    virtual void setCustomController(QAbstractSlider *controller);
    int getAngle();

public slots:
    virtual void addPixmap (const QPixmap &);
    virtual void setNumber(int);
    virtual void setPixmap(int index, QPixmap & pixmap);
    virtual void setPixmap(int index, QString path);
    virtual void setCurrentPixmap(QPixmap & pixmap);
    virtual void setCurrentPixmap(QString path);
    virtual void setView(int view);
    virtual void setAngle(int angle);

signals:
    virtual void viewChanged(int view);

protected:
    virtual void paintEvent ( QPaintEvent * event );
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void wheelEvent(QWheelEvent * event);
    
private slots:
    void on_slider_valueChanged(int value);

private:
    void update_controller();

    Ui::QTurntable *ui;
    QAbstractSlider *controller;
    QVector<QPixmap> pixmaps;
    int currentPixmap;
    float zoomFactor;

    int lastWidth,lastHeight,lastCurrent;
    bool needRedraw;
    int xclick, clickFrame;
};

#endif // QTURNTABLE_H
*/
