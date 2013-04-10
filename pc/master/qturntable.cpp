#include "qturntable.h"
#include "ui_qturntable.h"

QTurntable::QTurntable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QTurntable)
{
    ui->setupUi(this);
    currentPixmap = 0;
    pixmaps = new QList<QPixmap>();
}

QTurntable::~QTurntable()
{
    delete ui;
}

void QTurntable::paintEvent(QPaintEvent *event)
{
    QPixmap current = pixmaps->value(currentPixmap);
    if(!current.isNull())
        this->ui->displayer->setPixmap(current.scaled(ui->displayer->width(), ui->displayer->height(), Qt::KeepAspectRatio));
    QWidget::paintEvent(event);
}

void QTurntable::addPixmap (const QPixmap & pix)
{
    pixmaps->append(pix);
    ui->slider->setMaximum(pixmaps->length()-1);
}

void QTurntable::on_slider_sliderMoved(int position)
{
    currentPixmap = position;
}
