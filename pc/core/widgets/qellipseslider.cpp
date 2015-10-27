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

    ui->sideBlockSelection->addItem(tr("Block A"), QVariant(15));
    ui->sideBlockSelection->addItem(tr("Block B"), QVariant(12));
    ui->sideBlockSelection->addItem(tr("Custom Block"), QVariant(-1));
    ui->sideBlockSize->setProperty("custom", QVariant(-1)); // Remind the custom value

    ui->frontBlockSelection->addItem(tr("Block A"), QVariant(10));
    ui->frontBlockSelection->addItem(tr("Block C"), QVariant(7));
    ui->frontBlockSelection->addItem(tr("Custom Block"), QVariant(-1));
    ui->frontBlockSize->setProperty("custom", QVariant(-1)); // Remind the custom value

    m_valueName = tr("perimeter");
    ui->mainGroup->setTitle(m_valueName);
    //ui->warningLabel->setVisible(false);

    setFrontSize(150);
    setSideSize(150);

    m_lastFrontMotor = -1;
    m_lastSideMotor = -1;
    updateSlidersPositions();

    ui->frontMotorSlider->setBoundEditable(true);
    ui->frontMotorSlider->setBoundLimits(1000, 2000);
    ui->sideMotorSlider->setBoundEditable(true);
    ui->sideMotorSlider->setBoundLimits(1000,2000);
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
    ui->frontBlockSize->setValue(frontBlockOffset);
    if(ui->frontBlockSize->isEnabled()) // User entered the value
        ui->frontBlockSize->setProperty("custom", QVariant(frontBlockOffset));
    m_frontBlockOffset = frontBlockOffset;

    if(!perimeterLocked())
        setFrontMotorValue(m_lastFrontMotor);

    updateSlidersPositions();
}

void QEllipseSlider::setBaseOffset(int sideOffset, int frontOffset)
{
    setSideBaseOffset(sideOffset);
    setFrontBaseOffset(frontOffset);
}

void QEllipseSlider::setBlockOffset(int sideOffset, int frontOffset)
{
    setSideBlockOffset(sideOffset);
    setFrontBlockOffset(frontOffset);
}

int QEllipseSlider::sideBlockOffset() const
{
    return m_sideBlockOffset;
}

void QEllipseSlider::setSideBlockOffset(int sideBlockOffset)
{
    ui->sideBlockSize->setValue(sideBlockOffset);
    if(ui->sideBlockSize->isEnabled()) // User entered the value
        ui->sideBlockSize->setProperty("custom", QVariant(sideBlockOffset));
    m_sideBlockOffset = sideBlockOffset;
    if(!perimeterLocked())
        setSideMotorValue(m_lastSideMotor);

    updateSlidersPositions();
}

int QEllipseSlider::frontBaseOffset() const
{
    return m_frontBaseOffset;
}

void QEllipseSlider::setFrontBaseOffset(int frontBaseOffset)
{
    m_frontBaseOffset = frontBaseOffset;
    if(!perimeterLocked())
        setFrontMotorValue(m_lastFrontMotor);
    updateSlidersPositions();
}

int QEllipseSlider::sideBaseOffset() const
{
    return m_sideBaseOffset;
}

void QEllipseSlider::setSideBaseOffset(int sideBaseOffset)
{
    m_sideBaseOffset = sideBaseOffset;
    if(!perimeterLocked())
        setSideMotorValue(m_lastSideMotor);
    updateSlidersPositions();
}

void QEllipseSlider::setSideSliderEnabled(bool enabled)
{
    ui->sideGroup->setEnabled(enabled);
}

void QEllipseSlider::setFrontSliderEnabled(bool enabled)
{
    ui->frontGroup->setEnabled(enabled);
}

void QEllipseSlider::setBoundEditEnabled(bool enabled)
{
    ui->frontMotorSlider->setBoundEditable(enabled);
    ui->sideMotorSlider->setBoundEditable(enabled);
}

int QEllipseSlider::maxSideMotor() const
{
    return m_maxSideMotor;
}

void QEllipseSlider::setMaxSideMotor(int maxSideMotor)
{
    m_maxSideMotor = maxSideMotor;
}

void QEllipseSlider::setName(QString name)
{
    m_valueName = name;
    ui->mainGroup->setTitle(name);
}

int QEllipseSlider::getSideOffset()
{
    return m_sideBaseOffset+m_sideBlockOffset;
}

int QEllipseSlider::getFrontOffset()
{
    return m_frontBaseOffset+m_frontBlockOffset;
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
    int side,front;
    side = round(sideSize()-getSideOffset());
    front = round(frontSize()-getFrontOffset());
    if(side != m_lastSideMotor) {
        emit sideMotorValueChanged(side);
        m_lastSideMotor = side;
    }
    if(front != m_lastFrontMotor) {
        emit frontMotorValueChanged(front);
        m_lastFrontMotor = front;
    }

    // Block signals to avoid ping pong of rounded values
    ui->sideMotorSlider->blockSignals(true);
    ui->sideMotorSpinBox->blockSignals(true);
    ui->frontMotorSlider->blockSignals(true);
    ui->frontMotorSpinBox->blockSignals(true);
    ui->perimeterSlider->blockSignals(true);
    ui->perimeterSpinBox->blockSignals(true);

    // Do the update
    ui->sideMotorSlider->setValue(side);
    ui->sideMotorSpinBox->setValue(side);
    ui->frontMotorSlider->setValue(front);
    ui->frontMotorSpinBox->setValue(front);
    ui->perimeterSlider->setValue(round(perimeter()));
    ui->perimeterSpinBox->setValue(round(perimeter()));

    // Release the signals
    ui->sideMotorSlider->blockSignals(false);
    ui->sideMotorSpinBox->blockSignals(false);
    ui->frontMotorSlider->blockSignals(false);
    ui->frontMotorSpinBox->blockSignals(false);
    ui->perimeterSlider->blockSignals(false);
    ui->perimeterSpinBox->blockSignals(false);
    if(side < ui->sideMotorSlider->minimum() || side > ui->sideMotorSlider->maximum()
            || front < ui->frontMotorSlider->minimum() || front > ui->frontMotorSlider->maximum())
        ui->warningLabel->setText(tr("Unreachable %1").arg(m_valueName.toLower()));
    else
        ui->warningLabel->setText("");
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
    double side = 0.225079*sqrt((p*p*x*x)/(x*x + 1.0));
    double front = side/x;
    setSideSize(side);
    setFrontSize(front);
}

QString QEllipseSlider::sideBlockName() const
{
    return ui->sideBlockSelection->currentText();
}

QString QEllipseSlider::frontBlockName() const
{
    return ui->sideBlockSelection->currentText();
}

int QEllipseSlider::sideMotorValue()
{
    return round(sideSize()-getSideOffset());
}

int QEllipseSlider::frontMotorValue()
{
    return round(frontSize()-getFrontOffset());
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

void QEllipseSlider::on_sideBlockSelection_currentIndexChanged(int index)
{
    int offset = ui->sideBlockSelection->itemData(index).toInt();
    if(offset == -1) {
        ui->sideBlockSize->setEnabled(true);
        if(ui->sideBlockSize->property("custom").toInt() != -1)
            ui->sideBlockSize->setValue(ui->sideBlockSize->property("custom").toInt());
    } else {
        ui->sideBlockSize->setEnabled(false);
        ui->sideBlockSize->setValue(offset);
    }
}

void QEllipseSlider::on_frontBlockSelection_currentIndexChanged(int index)
{
    int offset = ui->frontBlockSelection->itemData(index).toInt();
    if(offset == -1) {
        ui->frontBlockSize->setEnabled(true);
        if(ui->frontBlockSize->property("custom").toInt() != -1)
            ui->frontBlockSize->setValue(ui->frontBlockSize->property("custom").toInt());
    } else {
        ui->frontBlockSize->setEnabled(false);
        ui->frontBlockSize->setValue(offset);
    }
}
