#include "qlinkedboundedsliders.h"
#include "ui_qlinkedboundedsliders.h"

QLinkedBoundedSliders::QLinkedBoundedSliders(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QLinkedBoundedSliders)
{
    ui->setupUi(this);

    connect(ui->slider1, &QBoundedSlider::valueChanged, [=](int newValue){
        if(ui->linkButton->isChecked())
            ui->slider2->setValue(newValue);
    });

    connect(ui->slider2, &QBoundedSlider::valueChanged, [=](int newValue){
        if(ui->linkButton->isChecked())
            ui->slider1->setValue(newValue);
    });
}

QLinkedBoundedSliders::~QLinkedBoundedSliders()
{
    delete ui;
}

int QLinkedBoundedSliders::slider1Value()
{
    return ui->slider1->value();
}

int QLinkedBoundedSliders::slider2Value()
{
    return ui->slider2->value();
}

void QLinkedBoundedSliders::setBoundEditable(bool editable)
{
    ui->slider1->setBoundEditable(editable);
    ui->slider2->setBoundEditable(editable);
}

void QLinkedBoundedSliders::setSlider1Bounds(int min, int max)
{
    ui->slider1->setLowerBound(min);
    ui->slider1->setUpperBound(max);
}

void QLinkedBoundedSliders::setSlider2Bounds(int min, int max)
{
    ui->slider2->setLowerBound(min);
    ui->slider2->setUpperBound(max);
}

void QLinkedBoundedSliders::setGlobalBounds(int min, int max)
{
    ui->slider1->setBoundLimits(min, max);
    ui->slider2->setBoundLimits(min, max);
}
