#include "qphotodisplayer.h"

QPhotoDisplayer::QPhotoDisplayer(QWidget *parent) :
    QLabel(parent)
{
    QSizePolicy policy;
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    policy.setVerticalPolicy(QSizePolicy::Preferred);
    setSizePolicy(policy);
    original_pixmap = 0;
}

void QPhotoDisplayer::paintEvent(QPaintEvent *event)
{
    if (pixmap())
        QLabel::setPixmap(original_pixmap->scaled(width(),height(),Qt::KeepAspectRatio));
    QLabel::paintEvent(event);
}

void QPhotoDisplayer::setPixmap(const QPixmap &new_pixmap)
{
    if(original_pixmap)
        delete original_pixmap;
    original_pixmap = new QPixmap(new_pixmap);
    QLabel::setPixmap(new_pixmap);
}
