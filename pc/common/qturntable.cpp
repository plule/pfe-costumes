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
    m_fit = false;
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

int QTurntable::getNumber()
{
    return m_pixmaps.size();
}

int QTurntable::getView()
{
    return m_current;
}

QString QTurntable::getCurrentFileName()
{
    return m_pixmaps.at(m_current).first;
}

void QTurntable::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    if(m_fit)
        fitInView();
}

void QTurntable::zoom(int factor)
{
    m_fit = false;
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

void QTurntable::loadDirs(QList<QDir> dirs, bool force)
{
    relativePaths = dirs;
    QSet<QString> filenamesSet;
    foreach(QDir dir, dirs)
        foreach(QString file, dir.entryList(QDir::Files, QDir::Name))
            filenamesSet.insert(file);
    QStringList files = filenamesSet.toList();
    qSort(files);
    setNumber(files.size());

    if(files.size()>0) {
        emit loadStart(tr("Loading 360° view"), files.size());
        for(int i=0; i<files.size(); i++) {
            setPicture(i, files.at(i));
            emit loadUpdate(i);
        }
        emit loadComplete();
    }
    if(m_current == -1 || m_current >= files.size())
        setView(0);
    if(m_fit)
        fitInView();
}

int QTurntable::computeZoom()
{
    qreal sc = transform().m11();
    return sc*100;
}

QString QTurntable::getPathOf(QString filename)
{
    if(!filename.startsWith(":")) {
        foreach(QDir dir, relativePaths)
            if(QFile::exists(dir.absoluteFilePath(filename)))
                return dir.absoluteFilePath(filename);
    }
    return filename;
}

void QTurntable::setZoom(int zoom)
{
    m_fit = false;
    if(zoom != m_zoom) {
        qreal sc = (qreal)zoom / 100.0;
        resetMatrix();
        scale(sc,sc);
        m_zoom = zoom;
        emit zoomChanged(m_zoom);
    }
}

void QTurntable::zoomIn()
{
    zoom(1);
}

void QTurntable::zoomOut()
{
    zoom(-1);
}

void QTurntable::setNumber(int n)
{
    m_pixmaps.resize(n);
    if(n == 0)
        m_current_pixmap->setPixmap(QPixmap());
}

void QTurntable::addPicture(QString path)
{
    QPixmap pic(getPathOf(path));
    m_pixmaps.append(QPair<QString,QPixmap>(path,pic));
    setView(getNumber()-1);
}

void QTurntable::setPicture(int index, QString path)
{
    QPixmap pic(getPathOf(path));
    if(index > m_pixmaps.size())
        setNumber(index+1);
    m_pixmaps[index] = QPair<QString,QPixmap>(path,pic);
    if(index == m_current)
    {
        m_current_pixmap->setPixmap(pic);
    }
    if(m_current < 0)
        setView(0);
}

void QTurntable::setCurrentPicture(QString path)
{
    setPicture(m_current, path);
    if(m_fit)
        fitInView();
}

void QTurntable::setView(int view)
{
    if(view < m_pixmaps.size() && view != m_current) {
        m_current = view;
        m_current_pixmap->setPixmap(m_pixmaps[m_current].second);
        if(m_fit)
            fitInView();
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
    m_fit = true;
}

void QTurntable::resetScale()
{
    QGraphicsView::resetMatrix();
    m_zoom = 100;
    m_fit = false;
}
