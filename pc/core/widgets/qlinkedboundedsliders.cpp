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

void QLinkedBoundedSliders::setSlider1Name(QString name)
{
    ui->slider1->setName(name);
}

void QLinkedBoundedSliders::setSlider2Name(QString name)
{
    ui->slider2->setName(name);
}

QBoundedSlider *QLinkedBoundedSliders::slider1()
{
    return ui->slider1;
}

QBoundedSlider *QLinkedBoundedSliders::slider2()
{
    return ui->slider2;
}

bool QLinkedBoundedSliders::getLinked()
{
    return ui->linkButton->isChecked();
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

void QLinkedBoundedSliders::setLinked(bool linked)
{
    ui->linkButton->setChecked(linked);
}
