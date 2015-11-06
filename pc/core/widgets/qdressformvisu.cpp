#include "qdressformvisu.h"
#include "ui_qdressformvisu.h"

QDressFormVisu::QDressFormVisu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QDressFormVisu)
{
    ui->setupUi(this);
    ui->BustBackLabel->setVisible(false);
    ui->BustFrontLabel->setVisible(false);
    ui->BustLeftLabel->setVisible(false);
    ui->BustRightLabel->setVisible(false);
    ui->HipsBackLabel->setVisible(false);
    ui->HipsFrontLabel->setVisible(false);
    ui->HipsRightLabel->setVisible(false);
    ui->HipsLeftLabel->setVisible(false);
    ui->WaistBackLabel->setVisible(false);
    ui->WaistFrontLabel->setVisible(false);
    ui->WaistLeftLabel->setVisible(false);
    ui->WaistRightLabel->setVisible(false);
}

QDressFormVisu::~QDressFormVisu()
{
    delete ui;
}

void QDressFormVisu::setBustFrontSelected(bool s)
{
    ui->BustFrontLabel->setVisible(s);
}

void QDressFormVisu::setBustBackSelected(bool s)
{
    ui->BustBackLabel->setVisible(s);
}

void QDressFormVisu::setBustLeftSelected(bool s)
{
    ui->BustLeftLabel->setVisible(s);
}

void QDressFormVisu::setBustRightSelected(bool s)
{
    ui->BustRightLabel->setVisible(s);
}

void QDressFormVisu::setWaistFrontSelected(bool s)
{
    ui->WaistFrontLabel->setVisible(s);
}

void QDressFormVisu::setWaistBackSelected(bool s)
{
    ui->WaistBackLabel->setVisible(s);
}

void QDressFormVisu::setWaistLeftSelected(bool s)
{
    ui->WaistLeftLabel->setVisible(s);
}

void QDressFormVisu::setWaistRightSelected(bool s)
{
    ui->WaistRightLabel->setVisible(s);
}

void QDressFormVisu::setHipsFrontSelected(bool s)
{
    ui->HipsFrontLabel->setVisible(s);
}

void QDressFormVisu::setHipsBackSelected(bool s)
{
    ui->HipsBackLabel->setVisible(s);
}

void QDressFormVisu::setHipsLeftSelected(bool s)
{
    ui->HipsLeftLabel->setVisible(s);
}

void QDressFormVisu::setHipsRightSelected(bool s)
{
    ui->HipsRightLabel->setVisible(s);
}
