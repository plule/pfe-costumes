#include "qellipseslider.h"
#include "ui_qellipseslider.h"
#include <cmath>

QEllipseSlider::QEllipseSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QEllipseSlider)
{
    ui->setupUi(this);
    m_frontBaseOffset = 124; // TODO adapter à conception
    m_sideBaseOffset = 100; // TODO adapter à conception
    m_frontBlockOffset = 10; // TODO liste de blocks
    m_sideBlockOffset = 15; // TODO liste de blocks

    ui->warningLabel->setVisible(false);

    m_frontSize = 150;
    m_sideSize = 180;
}

QEllipseSlider::~QEllipseSlider()
{
    delete ui;
}

int QEllipseSlider::frontBlockOffset() const
{
    return m_frontBlockOffset;
}

void QEllipseSlider::setFrontBlockOffset(int frontBlockOffset)
{
    m_frontBlockOffset = frontBlockOffset;
}

void QEllipseSlider::setBaseOffset(int sideOffset, int frontOffset)
{
    m_sideBaseOffset = sideOffset;
    m_frontBaseOffset = frontOffset;
}

void QEllipseSlider::setBlockOffset(int sideOffset, int frontOffset)
{
    m_sideBlockOffset = sideOffset;
    m_frontBlockOffset = frontOffset;
}

int QEllipseSlider::sideBlockOffset() const
{
    return m_sideBlockOffset;
}

void QEllipseSlider::setSideBlockOffset(int sideBlockOffset)
{
    m_sideBlockOffset = sideBlockOffset;
}

int QEllipseSlider::frontBaseOffset() const
{
    return m_frontBaseOffset;
}

void QEllipseSlider::setFrontBaseOffset(int frontBaseOffset)
{
    m_frontBaseOffset = frontBaseOffset;
}

int QEllipseSlider::sideBaseOffset() const
{
    return m_sideBaseOffset;
}

void QEllipseSlider::setSideBaseOffset(int sideBaseOffset)
{
    m_sideBaseOffset = sideBaseOffset;
}

int QEllipseSlider::maxSideMotor() const
{
    return m_maxSideMotor;
}

void QEllipseSlider::setMaxSideMotor(int maxSideMotor)
{
    m_maxSideMotor = maxSideMotor;
}

int QEllipseSlider::getSideOffset()
{
    return m_sideBaseOffset+m_sideBlockOffset;
}

int QEllipseSlider::getFrontOffset()
{
    return m_frontBaseOffset+m_sideBlockOffset;
}

bool QEllipseSlider::perimeterLocked()
{
    return ui->lockValueButton->isChecked();
}

void QEllipseSlider::setSideMotorValue(int value)
{
    setSideSize(value+getSideOffset(), perimeterLocked());
    updateSlidersPositions();
}

void QEllipseSlider::setFrontMotorValue(int value)
{
    setFrontSize(value+getFrontOffset(), perimeterLocked());
    updateSlidersPositions();
}

void QEllipseSlider::onPerimeterChanged(int value)
{
    setPerimeter(value);
    updateSlidersPositions();
}

void QEllipseSlider::updateSlidersPositions()
{
    int side = sideSize()-getSideOffset();
    int front = frontSize()-getFrontOffset();
    if(!ui->sideMotorSlider->isSliderDown())
        ui->sideMotorSlider->setValue(side);
    if(!ui->frontMotorSlider->isSliderDown())
        ui->frontMotorSlider->setValue(front);
    if(!ui->valueSlider->isSliderDown())
        ui->valueSlider->setValue(perimeter());
    if(side < ui->sideMotorSlider->minimum() || side > ui->sideMotorSlider->maximum()
            || front < ui->frontMotorSlider->minimum() || front > ui->frontMotorSlider->maximum())
        ui->warningLabel->setVisible(true);
    else
        ui->warningLabel->setVisible(false);
}

int QEllipseSlider::maxFrontMotor() const
{
    return m_maxFrontMotor;
}

void QEllipseSlider::setMaxFrontMotor(int maxFrontMotor)
{
    m_maxFrontMotor = maxFrontMotor;
}


double QEllipseSlider::calculateEllipsePerimeter(double a, double b)
{
    return M_PI*sqrt((2.0*(a*a+b*b)));
    //return 0.1*M_PI*(3.0*(a+b) - sqrt((3*a+b)*(a+3*b)));
}

double QEllipseSlider::perimeter() const
{
    double a = sideSize();
    double b = frontSize();
    qDebug() << "-----------";
    qDebug() << a;
    qDebug() << b;
    return M_PI*sqrt((2.0*(a*a+b*b)));
    //return M_PI*(3.0*(a+b) - sqrt((3.0*a+b)*(a+3.0*b)));
}

void QEllipseSlider::setPerimeter(double p)
{
    double x;
    if(sideSize() <= 0 || frontSize() <= 0)
        x = 1;
    else
        x = sideSize()/frontSize();
    double side = 0.225079*sqrt((p*p*x*x)/(x*x + 1.0));
    double front = side/x;
    setSideSize(side);
    setFrontSize(front);
}

double QEllipseSlider::sideSize() const
{
    return m_sideSize;
}

void QEllipseSlider::setSideSize(double sideSize, bool keepPerimeter)
{
    if(keepPerimeter) {
        double p = perimeter();
        setFrontSize(calculateEllipseParameter(p, sideSize));
    }
    m_sideSize = sideSize;
}

double QEllipseSlider::frontSize() const
{
    return m_frontSize;
}

void QEllipseSlider::setFrontSize(double frontSize, bool keepPerimeter)
{
    if(keepPerimeter) {
        double p = perimeter();
        setSideSize(calculateEllipseParameter(p, frontSize));
    }
    m_frontSize = frontSize;
}

double QEllipseSlider::calculateEllipseParameter(double p, double b)
{
    return sqrt( p*p / (2.0*M_PI*M_PI) - b*b );
}
