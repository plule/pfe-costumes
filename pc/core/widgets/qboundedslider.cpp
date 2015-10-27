#include "qboundedslider.h"
#include "ui_qboundedslider.h"

QBoundedSlider::QBoundedSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QBoundedSlider)
{
    ui->setupUi(this);

    // slider signal forwarding
    connect(ui->slider, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
    connect(ui->slider, SIGNAL(rangeChanged(int,int)), this, SIGNAL(rangeChanged(int,int)));

    // spin to slider signals
    connect(ui->lowerBoundSpin, SIGNAL(valueChanged(int)),
            this, SLOT(setMinimum(int)));
    connect(ui->upperBoundSpin, SIGNAL(valueChanged(int)),
            this, SLOT(setMaximum(int)));

    // spin to spin signals
    connect(ui->lowerBoundSpin, SIGNAL(valueChanged(int)),
            this, SLOT(setUpperboundMinimum(int)));
    connect(ui->upperBoundSpin, SIGNAL(valueChanged(int)),
            this, SLOT(setLowerboundMaximum(int)));

    setEditable(false);
}

QBoundedSlider::~QBoundedSlider()
{
    delete ui;
}

int QBoundedSlider::maximum() const
{
    return ui->slider->maximum();
}

int QBoundedSlider::minimum() const
{
    return ui->slider->minimum();
}

int QBoundedSlider::value() const
{
    return ui->slider->value();
}

void QBoundedSlider::setRange(int min, int max)
{
    ui->slider->setRange(min, max);
}

void QBoundedSlider::setValue(int value)
{
    ui->slider->setValue(value);
}

void QBoundedSlider::setMaximum(int max)
{
    ui->slider->setMaximum(max);
}

void QBoundedSlider::setMinimum(int min)
{
    ui->slider->setMinimum(min);
}

void QBoundedSlider::setAbsoluteMaximum(int max)
{
    ui->upperBoundSpin->setMaximum(max);
}

void QBoundedSlider::setAbsoluteMinimum(int min)
{
    ui->lowerBoundSpin->setMinimum(min);
}

void QBoundedSlider::setLowerboundMaximum(int max)
{
    ui->lowerBoundSpin->setMaximum(max);
}

void QBoundedSlider::setUpperboundMinimum(int min)
{
    ui->upperBoundSpin->setMinimum(min);
}

void QBoundedSlider::setEditable(bool editable)
{
    m_editable = editable;
    ui->lowerBoundSpin->setVisible(editable);
    ui->upperBoundSpin->setVisible(editable);
}

bool QBoundedSlider::editable() const
{
    return m_editable;
}
