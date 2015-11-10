#include "qdressformvisu.h"
#include "ui_qdressformvisu.h"

QDressFormVisu::QDressFormVisu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QDressFormVisu)
{
    ui->setupUi(this);
    ui->BustFrontLabel->setVisible(false);
    ui->BustSideLabel->setVisible(false);
    ui->HipsFrontLabel->setVisible(false);
    ui->HipsSideLabel->setVisible(false);
    ui->WaistFrontLabel->setVisible(false);
    ui->WaistSideLabel->setVisible(false);
}

QDressFormVisu::~QDressFormVisu()
{
    delete ui;
}

void QDressFormVisu::setBustFrontSelected(bool s)
{
    ui->BustFrontLabel->setVisible(s);
}

void QDressFormVisu::setBustSideSelected(bool s)
{
    ui->BustSideLabel->setVisible(s);
}

void QDressFormVisu::setWaistFrontSelected(bool s)
{
    ui->WaistFrontLabel->setVisible(s);
}

void QDressFormVisu::setWaistSideSelected(bool s)
{
    ui->WaistSideLabel->setVisible(s);
}

void QDressFormVisu::setHipsFrontSelected(bool s)
{
    ui->HipsFrontLabel->setVisible(s);
}

void QDressFormVisu::setHipsSideSelected(bool s)
{
    ui->HipsSideLabel->setVisible(s);
}
