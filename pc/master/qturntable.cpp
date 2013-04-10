#include "qturntable.h"
#include "ui_qturntable.h"

QTurntable::QTurntable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QTurntable)
{
    ui->setupUi(this);
    currentPixmap = 0;
    //pixmaps = new QVector<QPixmap>();
    controller = ui->slider;
    lastWidth = -1;
    lastHeight = -1;
    lastCurrent = -1;
}

QTurntable::~QTurntable()
{
    delete ui;
}

void QTurntable::paintEvent(QPaintEvent *event)
{
    if(currentPixmap != lastCurrent || ui->displayer->width() != lastWidth || ui->displayer->height() != lastHeight)
    {
        QPixmap current = pixmaps.value(currentPixmap);
        if(!current.isNull())
            this->ui->displayer->setPixmap(current.scaled(ui->displayer->width(), ui->displayer->height(), Qt::KeepAspectRatio));
        else
            this->ui->displayer->clear();
        lastCurrent = currentPixmap;
        lastWidth = ui->displayer->width();
        lastHeight = ui->displayer->height();
    }
    QWidget::paintEvent(event);
}

void QTurntable::addPixmap (const QPixmap & pix)
{
    pixmaps.append(pix);
    update_controller();
}

void QTurntable::setNumber(int number)
{
    pixmaps.resize(number);
    update_controller();
}

void QTurntable::setPixmap(int index, QPixmap & pixmap)
{
    if(index > pixmaps.size())
        setNumber(index+1);
    pixmaps[index] = pixmap;
    if(index == currentPixmap)
        this->update();
}

void QTurntable::setPixmap(int index, QString path)
{
    QPixmap pic(path);
    setPixmap(index, pic);
}

void QTurntable::setCurrentPixmap(QPixmap &pixmap)
{
    setPixmap(currentPixmap, pixmap);
}

void QTurntable::setCurrentPixmap(QString path)
{
    setPixmap(currentPixmap, path);
}

void QTurntable::on_slider_sliderMoved(int position)
{
    currentPixmap = position;
    this->update();
}

void QTurntable::update_controller()
{
    controller->setMinimum(0);
    if(pixmaps.size() > 0)
        controller->setMaximum(pixmaps.size()-1);
    else
        controller->setMaximum(0);
}

void QTurntable::showController(bool show)
{
    this->ui->slider->setVisible(show);
}

void QTurntable::setCustomController(QAbstractSlider *ext_controller)
{
    if(ext_controller) {
        ui->slider->setVisible(false);
        controller = ext_controller;
        connect(controller, SIGNAL(sliderMoved(int)), this, SLOT(on_slider_sliderMoved(int)));
        update_controller();
    } else {
        ui->slider->setVisible(true);
        controller = ui->slider;
        update_controller();
    }
}
