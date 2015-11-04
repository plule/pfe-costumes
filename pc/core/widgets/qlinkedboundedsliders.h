#ifndef QLINKEDBOUNDEDSLIDERS_H
#define QLINKEDBOUNDEDSLIDERS_H

#include <QWidget>
#include <QTimer>
#include "widgets/qboundedslider.h"

namespace Ui {
class QLinkedBoundedSliders;
}

class QLinkedBoundedSliders : public QWidget
{
    Q_OBJECT

public:
    explicit QLinkedBoundedSliders(QWidget *parent = 0);
    ~QLinkedBoundedSliders();

    int slider1Value();
    int slider2Value();
    void setSlider1Name(QString name);
    void setSlider2Name(QString name);
    QBoundedSlider *slider1();
    QBoundedSlider *slider2();

public slots:
    void setBoundEditable(bool editable);
    void setSlider1Bounds(int min, int max);
    void setSlider2Bounds(int min, int max);
    void setGlobalBounds(int min, int max);
    void setLinked(bool linked);

signals:
    void slider1ValueChanged(int value);
    void slider2ValueChanged(int value);
    void slider1UpperBoundValueChanged(int value);
    void slider2UpperBoundValueChanged(int value);
    void slider1LowerBoundValueChanged(int value);
    void slider2LowerBoundValueChanged(int value);

private:
    Ui::QLinkedBoundedSliders *ui;
};

#endif // QLINKEDBOUNDEDSLIDERS_H
