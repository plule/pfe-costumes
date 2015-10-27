#ifndef QBOUNDEDSLIDER_H
#define QBOUNDEDSLIDER_H

#include <QWidget>

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


public slots:
    void setRange(int min, int max);
    void setValue(int value);
    void setMaximum(int max);
    void setMinimum(int min);
    void setAbsoluteMaximum(int max);
    void setAbsoluteMinimum(int min);
    void setEditable(bool editable);

protected slots:
    void setUpperboundMinimum(int min);
    void setLowerboundMaximum(int max);

signals:
    void rangeChanged(int min, int max);
    void valueChanged(int value);

private:
    Ui::QBoundedSlider *ui;
    bool m_editable;
};

#endif // QBOUNDEDSLIDER_H
