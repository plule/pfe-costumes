#ifndef QBOUNDEDSLIDER_H
#define QBOUNDEDSLIDER_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class QBoundedSlider;
}

class QBoundedSlider : public QWidget
{
    Q_OBJECT

public:
    explicit QBoundedSlider(QWidget *parent = 0);
    ~QBoundedSlider();

    int maximum() const;
    int minimum() const;
    int value() const;
    bool editable() const;
    void setName(QString name);
    bool hovered();


public slots:
    void setRange(int min, int max);
    void setValue(int value);
    void setMaximum(int max);
    void setMinimum(int min);

    void setLowerBound(int value);
    void setUpperBound(int value);
    void setBoundEditable(bool editable);
    void setBoundLimits(int min, int max);
    void setOrientation(Qt::Orientation orientation);

signals:
    void rangeChanged(int min, int max);
    void valueChanged(int value);
    void lowerBoundChanged(int value);
    void upperBoundChanged(int value);
    void hoveredChanged(bool hovered);

protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);

private:
    Ui::QBoundedSlider *ui;
    bool m_editable;
    bool m_hovered;
};

#endif // QBOUNDEDSLIDER_H
