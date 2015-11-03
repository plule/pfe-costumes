#include "qslidergroup.h"
#include "ui_qslidergroup.h"

QSliderGroup::QSliderGroup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QSliderGroup)
{
    ui->setupUi(this);
}

QSliderGroup::~QSliderGroup()
{
    delete ui;
}

void QSliderGroup::setName(QString name)
{
    ui->group->setTitle(name);
}

QLinkedBoundedSliders *QSliderGroup::sliders1()
{
    return ui->linkedSliders1;
}

QLinkedBoundedSliders *QSliderGroup::sliders2()
{
    return ui->linkedSliders2;
}
