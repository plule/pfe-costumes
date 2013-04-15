#include "qturntable.h"

QTurntable::QTurntable(QWidget *parent) :
    QGraphicsView(parent)
{
    this->setScene(new QGraphicsScene());
    m_current_pixmap = this->scene()->addPixmap(QPixmap());
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
        m_current_pixmap->setPixmap(m_pixmaps[m_current]);
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
    m_current = view;
    m_current_pixmap->setPixmap(m_pixmaps[m_current]);
}

void QTurntable::setAngle(int angle)
{
    setView(m_pixmaps.size() * angle / 360);
}
