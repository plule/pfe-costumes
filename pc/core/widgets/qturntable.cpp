#include "qturntable.h"

QTurntable::QTurntable(QWidget *parent) :
    QGraphicsView(parent)
{
    this->setScene(new QGraphicsScene());
    //m_current_pixmap = this->scene()->addPixmap(QPixmap());
    //m_current_pixmap->setTransformationMode(Qt::SmoothTransformation);
    m_current_view = -1;
    m_zoom = 1;
    m_zoom_step = 1.25;
    m_min_zoom = 1;
    m_max_zoom = 200;
    m_current_pixmap = 0;
    m_fit = false;

    m_resize_preview = true;
    m_preview_dimension.first = 2000;
    m_preview_dimension.second = 1500;
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

int QTurntable::getNumber()
{
    return m_pixmaps.size();
}

int QTurntable::getView()
{
    return m_current_view;
}

QString QTurntable::getCurrentFileName()
{
    return m_pixmaps.at(getView()).first;
}

void QTurntable::enableResizePreview(bool resize)
{
    m_resize_preview = resize;
}

void QTurntable::setPreviewSize(int width, int size)
{
    m_preview_dimension.first = width;
    m_preview_dimension.second = size;
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

void QTurntable::loadDirs(QList<QDir> dirs)
{
    relativePaths = dirs;
    QSet<QString> filenamesSet;
    foreach(QDir dir, dirs)
        foreach(QString file, dir.entryList(QDir::Files, QDir::Name))
            if(file.endsWith(".jpg"))
                filenamesSet.insert(file);
    QStringList files = filenamesSet.toList();
    qSort(files);
    setNumber(0); // ensure cleaning m_pixmaps
    setNumber(files.size());

    if(files.size()>0) {
        emit loadStart(tr("Loading 360° view"), files.size());
        for(int i=0; i<files.size(); i++) {
            setPicture(i, files.at(i));
            emit loadUpdate(i);
        }
        emit loadComplete();
    }
    if((m_current_view == -1) || (getView() >= files.size()))
        setView(0);
    if(m_fit)
        fitInView();
}

void QTurntable::setBackgroundColor(QColor color)
{
    setBackgroundBrush(QBrush(color));
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
    for(int i=0; i<m_pixmaps.size(); i++)
        delete m_pixmaps.value(i).second;
    m_pixmaps.resize(n);
    for(int i=0; i<n; i++) {
        m_pixmaps[i].second = new QGraphicsPixmapItem();
        m_pixmaps[i].second->setVisible(false);
        this->scene()->addItem(m_pixmaps[i].second);
    }
    m_current_pixmap = 0;
    emit numberChanged(n);
}

void QTurntable::addPicture(QString path)
{
    QPixmap pic = ExifPixmap::createPixmap(getPathOf(path));
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pic);
    this->scene()->addItem(item);
    m_pixmaps.append(QPair<QString,QGraphicsPixmapItem*>(path,item));
    setView(getNumber()-1);
}

void QTurntable::setPicture(int index, QString path)
{
    QPixmap pic = ExifPixmap::createPixmap(getPathOf(path));
    if(m_resize_preview)
        pic = pic.scaled(m_preview_dimension.first, m_preview_dimension.second, Qt::KeepAspectRatio);
    if(index >= m_pixmaps.size())
        setNumber(index+1);
    QFileInfo infos(getPathOf(path));
    m_pixmaps[index].first = infos.fileName();
    m_pixmaps[index].second->setPixmap(pic);
    if(index == getView()) {
        m_current_pixmap = m_pixmaps[index].second;
        m_current_pixmap->setVisible(true);
    }
    if(m_current_view < 0)
        setView(0);
}

void QTurntable::setCurrentPicture(QString path)
{
    setPicture(getView(), path);
    if(m_fit)
        fitInView();
}

void QTurntable::setView(int view)
{
    if(view < m_pixmaps.size() && view != getView()) {
        if(m_current_pixmap != 0) {
            m_current_pixmap->setVisible(false); // TODO segfault
        }
        m_current_pixmap = m_pixmaps[view].second;
        m_current_pixmap->setVisible(true);
        m_current_view = view;
        if(m_fit)
            fitInView();
        emit viewChanged(view);
    }
}

void QTurntable::setPictureAndView(int index, QString path)
{
    setPicture(index, path);
    setView(index);
}

void QTurntable::fitInView()
{
    if(this->items().size() > 0) {
        QGraphicsView::fitInView(this->items()[0], Qt::KeepAspectRatio);
        int new_zoom = computeZoom();
        if(new_zoom != m_zoom){
            m_zoom = new_zoom;
            emit zoomChanged(m_zoom);
        }
    }
    m_fit = true;
}

void QTurntable::resetScale()
{
    QGraphicsView::resetMatrix();
    m_zoom = 100;
    m_fit = false;
}
