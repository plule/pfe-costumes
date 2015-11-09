#include "qboundedslider.h"
#include "ui_qboundedslider.h"

QBoundedSlider::QBoundedSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QBoundedSlider)
{
    ui->setupUi(this);

    m_hovered = false;

    // slider signal forwarding
    connect(ui->slider, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
    connect(ui->slider, SIGNAL(rangeChanged(int,int)), this, SIGNAL(rangeChanged(int,int)));

    // spins signal forwarding
    connect(ui->lowerBoundSpin, SIGNAL(valueChanged(int)),
            this, SIGNAL(lowerBoundChanged(int)));
    connect(ui->upperBoundSpin, SIGNAL(valueChanged(int)),
            this, SIGNAL(upperBoundChanged(int)));

    setBoundEditable(false);
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

bool QBoundedSlider::hovered()
{
    return m_hovered;
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

void QBoundedSlider::setLowerBound(int value)
{
    ui->lowerBoundSpin->setValue(value);
}

void QBoundedSlider::setUpperBound(int value)
{
    ui->upperBoundSpin->setValue(value);
}

void QBoundedSlider::setBoundLimits(int min, int max)
{
    ui->lowerBoundSpin->setRange(min, max);
    ui->upperBoundSpin->setRange(min, max);
}

void QBoundedSlider::setBoundEditable(bool editable)
{
    m_editable = editable;
    ui->lowerBoundSpin->setVisible(editable);
    ui->upperBoundSpin->setVisible(editable);
}

void QBoundedSlider::setOrientation(Qt::Orientation orientation)
{
    ui->slider->setOrientation(orientation);
}

void QBoundedSlider::enterEvent(QEvent *event)
{
    m_hovered = true;
    emit hoveredChanged(true);
    QWidget::enterEvent(event);
}

void QBoundedSlider::leaveEvent(QEvent *event)
{
    m_hovered = false;
    emit hoveredChanged(false);
    QWidget::leaveEvent(event);
}

bool QBoundedSlider::editable() const
{
    return m_editable;
}
