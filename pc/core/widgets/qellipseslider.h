#ifndef QELLIPSESLIDER_H
#define QELLIPSESLIDER_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class QEllipseSlider;
}

class QEllipseSlider : public QWidget
{
    Q_OBJECT
    
public:
    explicit QEllipseSlider(QWidget *parent = 0);
    ~QEllipseSlider();

    /*
     * Constant width of the model
     */
    int sideBaseOffset() const;
    void setSideBaseOffset(int sideBaseOffset);

    /*
     * Constant depth of the model
     */
    int frontBaseOffset() const;
    void setFrontBaseOffset(int frontBaseOffset);

    /*
     * Size added by the sides block
     */
    int sideBlockOffset() const;
    void setSideBlockOffset(int sideBlockOffset);

    /*
     * Size added by the front block
     */
    int frontBlockOffset() const;
    void setFrontBlockOffset(int frontBlockOffset);

    void setBaseOffset(int sideOffset, int frontOffset);
    void setBlockOffset(int sideOffset, int frontOffset);

    int maxFrontMotor() const;
    void setMaxFrontMotor(int maxFrontMotor);

    int maxSideMotor() const;
    void setMaxSideMotor(int maxSideMotor);

    /*
     * Smallest size of the model
     */
    int getSideOffset();
    int getFrontOffset();

    /*
     * Perimeter should not vary (lock perimeter button)
     */
    bool perimeterLocked();

    /*
     * Current size of the model
     */
    double frontSize() const;
    void setFrontSize(double frontSize, bool keepPerimeter=false);

    double sideSize() const;
    void setSideSize(double sideSize, bool keepPerimeter=false);

    /*
     * Calculate perimeter
     */
    double perimeter() const;
    /*
     * Modify proportionnally the model to reach a perimeter
     */
    void setPerimeter(double p);

public slots:

protected slots:
    void setSideMotorValue(int value);
    void setFrontMotorValue(int value);
    void onPerimeterChanged(int value);
    void updateSlidersPositions();

signals:
    void motorValuesChanged(int sideMotor, int frontMotor);
    void sideMotorValueChanged(int value);
    void frontMotorValueChanged(int value);
    void valueChanged(int value);

protected:
    double calculateEllipsePerimeter(double side, double front);
    /*
     * Given the perimeter and a radius of an ellipse, get the other radius
     */
    double calculateEllipseParameter(double perimeter, double otherParameter);
    
private:
    Ui::QEllipseSlider *ui;
    int m_sideBaseOffset, m_frontBaseOffset, m_sideBlockOffset, m_frontBlockOffset;
    int m_maxFrontMotor, m_maxSideMotor;

    double m_sideSize, m_frontSize;
};

#endif // QELLIPSESLIDER_H
