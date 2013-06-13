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

    int sideBaseOffset() const;
    void setSideBaseOffset(int sideBaseOffset);

    int frontBaseOffset() const;
    void setFrontBaseOffset(int frontBaseOffset);

    int sideBlockOffset() const;
    void setSideBlockOffset(int sideBlockOffset);

    int frontBlockOffset() const;
    void setFrontBlockOffset(int frontBlockOffset);

    void setBaseOffset(int sideOffset, int frontOffset);
    void setBlockOffset(int sideOffset, int frontOffset);

    int maxFrontMotor() const;
    void setMaxFrontMotor(int maxFrontMotor);

    int maxSideMotor() const;
    void setMaxSideMotor(int maxSideMotor);

    int getSideOffset();
    int getFrontOffset();


    double getPerimeter();

    bool perimeterLocked();

    double frontSize() const;
    void setFrontSize(double frontSize, bool keepPerimeter=false);

    double sideSize() const;
    void setSideSize(double sideSize, bool keepPerimeter=false);

    double perimeter() const;
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
    double calculateEllipseParameter(double perimeter, double otherParameter);
    
private:
    Ui::QEllipseSlider *ui;
    int m_sideBaseOffset, m_frontBaseOffset, m_sideBlockOffset, m_frontBlockOffset;
    int m_maxFrontMotor, m_maxSideMotor;

    double m_sideSize, m_frontSize;
};

#endif // QELLIPSESLIDER_H
