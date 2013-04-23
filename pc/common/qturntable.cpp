#include "qturntable.h"

QTurntable::QTurntable(QWidget *parent) :
    QGraphicsView(parent)
{
    this->setScene(new QGraphicsScene());
    m_current_pixmap = this->scene()->addPixmap(QPixmap());
    m_current = -1;
    m_zoom = 1;
    m_zoom_step = 1.25;
    m_min_zoom = 1;
    m_max_zoom = 200;
}

void QTurntable::wheelEvent(QWheelEvent *e)
{
    int numSteps = e->delta() / 15 / 8;

    if (numSteps == 0) {
        e->ignore();
        return;
    }
    this->zoom(numSteps);
    e->accept();
}

int QTurntable::getZoomStep()
{
    return 360 / m_pixmaps.size();
}

int QTurntable::getAngleStep()
{
    return 3;
}

QString QTurntable::getPaths()
{
    return "yea";
}

void QTurntable::loadPaths(QString paths)
{
    this->paths = paths;
}

void QTurntable::zoom(int factor)
{
    qreal dfactor = pow(m_zoom_step, factor);
    if(transform().m11()*dfactor*100 > m_max_zoom)
        dfactor = (m_max_zoom)/(transform().m11()*100);
    if(transform().m11()*dfactor*100 < m_min_zoom)
        dfactor = (m_min_zoom)/(transform().m11()*100);
    scale(dfactor,dfactor);
    if(m_zoom != computeZoom()) {
        m_zoom = computeZoom();
        emit zoomChanged(m_zoom);
    }
}

int QTurntable::computeZoom()
{
    qreal sc = transform().m11();
    return sc*100;
}

void QTurntable::setZoom(int zoom)
{
    if(zoom != m_zoom) {
        qreal sc = (qreal)zoom / 100.0;
        resetMatrix();
        scale(sc,sc);
        m_zoom = zoom;
        emit zoomChanged(m_zoom);

    }
}

void QTurntable::addPicture(const QPixmap & pixmap)
{
    m_pixmaps.append(pixmap);
}

void QTurntable::setNumber(int n)
{
    m_pixmaps.resize(n);
}

void QTurntable::addPicture(QString path)
{
    QPixmap pic(path);
    addPicture(pic);
}

void QTurntable::setPicture(int index, QPixmap &pixmap)
{
    if(index > m_pixmaps.size())
        setNumber(index+1);
    m_pixmaps[index] = pixmap;
    if(index == m_current)
    {
        m_current_pixmap->setPixmap(pixmap);
    }
}

void QTurntable::setPicture(int index, QString path)
{
    QPixmap pic(path);
    setPicture(index, pic);
}

void QTurntable::setCurrentPicture(QPixmap &pixmap)
{
    setPicture(m_current, pixmap);
}

void QTurntable::setCurrentPicture(QString path)
{
    setPicture(m_current, path);
}

void QTurntable::setView(int view)
{
    if(view < m_pixmaps.size() && view != m_current) {
        m_current = view;
        m_current_pixmap->setPixmap(m_pixmaps[m_current]);
        emit angleChanged(360 * m_current / m_pixmaps.size());
    }
}

void QTurntable::setAngle(int angle)
{
    setView(round(((double)m_pixmaps.size() * (double)angle) / 360.0));
}

void QTurntable::fitInView()
{
    QGraphicsView::fitInView(this->items()[0], Qt::KeepAspectRatio);
    int new_zoom = computeZoom();
    if(new_zoom != m_zoom){
        m_zoom = new_zoom;
        emit zoomChanged(m_zoom);
    }
}

void QTurntable::resetScale()
{
    QGraphicsView::resetMatrix();
    m_zoom = 100;
}
