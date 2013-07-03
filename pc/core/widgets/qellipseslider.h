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
    Q_ENUMS(MOTOR_TYPE)
    
public:
    explicit QEllipseSlider(QWidget *parent = 0);
    ~QEllipseSlider();

    enum MOTOR_TYPE {
        FRONT,
        SIDE
    };

    /*
     * Constant width of the model
     */
    int sideBaseOffset() const;

    /*
     * Constant depth of the model
     */
    int frontBaseOffset() const;

    /*
     * Size added by the sides block
     */
    int sideBlockOffset() const;

    /*
     * Size added by the front block
     */
    int frontBlockOffset() const;

    void setBaseOffset(int sideOffset, int frontOffset);
    void setBlockOffset(int sideOffset, int frontOffset);

    int maxFrontMotor() const;
    void setMaxFrontMotor(int maxFrontMotor);

    int maxSideMotor() const;
    void setMaxSideMotor(int maxSideMotor);

    void setName(QString name);

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

    /*
     * Get current block name
     */
    QString sideBlockName() const;
    QString frontBlockName() const;

public slots:
    void setSideMotorValue(int value);
    void setFrontMotorValue(int value);
    void setFrontBlockOffset(int frontBlockOffset);
    void setSideBlockOffset(int sideBlockOffset);
    void setFrontBaseOffset(int frontBaseOffset);
    void setSideBaseOffset(int sideBaseOffset);
    /*
     * Enable or disable sliders
     */
    void setSideSliderEnabled(bool enabled);
    void setFrontSliderEnabled(bool enabled);


protected slots:
    void onPerimeterChanged(int value);
    void updateSlidersPositions();

signals:
    void sideMotorValueChanged(int value);
    void frontMotorValueChanged(int value);

protected:
    double calculateEllipsePerimeter(double side, double front);
    /*
     * Given the perimeter and a radius of an ellipse, get the other radius
     */
    double calculateEllipseParameter(double perimeter, double otherParameter);
    
private slots:
    void on_sideBlockSelection_currentIndexChanged(int index);

    void on_frontBlockSelection_currentIndexChanged(int index);

private:
    Ui::QEllipseSlider *ui;
    int m_sideBaseOffset, m_frontBaseOffset, m_sideBlockOffset, m_frontBlockOffset;
    int m_maxFrontMotor, m_maxSideMotor;
    QString m_valueName;

    double m_sideSize, m_frontSize;
    int m_lastSideMotor, m_lastFrontMotor;
};

#endif // QELLIPSESLIDER_H
