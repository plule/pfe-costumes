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

double QEllipseSlider::getPerimeter()
{
    return ui->valueSlider->value(); // is in cm
}

bool QEllipseSlider::valueLocked()
{
    return ui->lockValueButton->isChecked();
}

void QEllipseSlider::setSideMotorValue(int value)
{
    //if(ui->sideMotorSlider->isSliderDown()) {
        setSideSize(value, valueLocked());
        updateSlidersPositions();
    //}
/*    if(valueLocked()) {
        if(!ui->frontMotorSlider->isSliderDown()) { // avoid recursive propagation
            setFrontSlider(calculateEllipseParameter(getPerimeter(), getSideSize()));
        }
    } else {
        setPerimeterSlider(calculateEllipsePerimeter(getSideSize(), getFrontSize()));
    }*/
}

void QEllipseSlider::setFrontMotorValue(int value)
{
    //if(ui->frontMotorSlider->isSliderDown()) {
        setFrontSize(value, valueLocked());
        updateSlidersPositions();
    //}
/*    if(valueLocked()) {
        if(!ui->sideMotorSlider->isSliderDown()) {
            setSideSlider(calculateEllipseParameter(getPerimeter(), getFrontSize()));
        }
    } else {
        setPerimeterSlider(calculateEllipsePerimeter(getSideSize(), getFrontSize()));
    }*/
}

void QEllipseSlider::onValueChanged(int value)
{
    setPerimeter(value);
    updateSlidersPositions();
}

void QEllipseSlider::updateSlidersPositions()
{
    if(!ui->sideMotorSlider->isSliderDown())
        ui->sideMotorSlider->setValue(sideSize());
    if(!ui->frontMotorSlider->isSliderDown())
        ui->frontMotorSlider->setValue(frontSize());
    if(!ui->valueSlider->isSliderDown())
        ui->valueSlider->setValue(perimeter());
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
    qDebug() << x;
    double side = 0.225079*sqrt((p*p*x*x)/(x*x + 1.0));// / (M_SQRT2*M_PI);
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
