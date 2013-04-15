#ifndef QTURNTABLE_H
#define QTURNTABLE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>

class QTurntable : public QGraphicsView
{
    Q_OBJECT
public:
    explicit QTurntable(QWidget *parent = 0);
    
signals:
    
public slots:
    virtual void addPixmap (const QPixmap &);
    virtual void setNumber(int n);
    virtual void setPixmap(int index, QPixmap &pixmap);
    virtual void setPixmap(int index, QString path);
    virtual void setCurrentPixmap(QPixmap & pixmap);
    virtual void setCurrentPixmap(QString path);
    virtual void setView(int view);
    virtual void setAngle(int angle);
    

private:
    QVector<QPixmap> m_pixmaps;
    int m_current;
    QGraphicsPixmapItem *m_current_pixmap;
};

#endif // QTURNTABLE_H
