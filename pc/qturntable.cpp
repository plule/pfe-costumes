#include "qturntable.h"

QTurntable::QTurntable(QWidget *parent) :
    QGraphicsView(parent)
{
    this->setScene(new QGraphicsScene());
    m_current_pixmap = this->scene()->addPixmap(QPixmap());
    m_current = 0;
    m_zoom = 0;
}

void QTurntable::wheelEvent(QWheelEvent *e)
{
    int numSteps = e->delta() / 15 / 8;

    if (numSteps == 0) {
        e->ignore();
        return;
    }
    qreal sc = pow(1.25, numSteps); // I use scale factor 1.25
    this->zoom(sc);
    e->accept();
}

void QTurntable::zoom(qreal factor)
{
    scale(factor, factor);
}

void QTurntable::addPixmap(const QPixmap & pixmap)
{
    m_pixmaps.append(pixmap);
}

void QTurntable::setNumber(int n)
{
    m_pixmaps.resize(n);
}

void QTurntable::setPixmap(int index, QPixmap &pixmap)
{
    if(index > m_pixmaps.size())
        setNumber(index+1);
    m_pixmaps[index] = pixmap;
    if(index == m_current)
    {
        setView(index);
    }
}

void QTurntable::setPixmap(int index, QString path)
{
    QPixmap pic(path);
    setPixmap(index, pic);
}

void QTurntable::setCurrentPixmap(QPixmap &pixmap)
{
    setPixmap(m_current, pixmap);
}

void QTurntable::setCurrentPixmap(QString path)
{
    setPixmap(m_current, path);
}

void QTurntable::setView(int view)
{
    if(view < m_pixmaps.size()) {
        m_current = view;
        m_current_pixmap->setPixmap(m_pixmaps[m_current]);
    }
}

void QTurntable::setAngle(int angle)
{
    setView(m_pixmaps.size() * angle / 360);
}

void QTurntable::fitInView()
{
    QGraphicsView::fitInView(this->items()[0], Qt::KeepAspectRatio);
}

void QTurntable::resetScale()
{
    QGraphicsView::resetMatrix();
}
