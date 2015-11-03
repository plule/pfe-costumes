#ifndef QSLIDERGROUP_H
#define QSLIDERGROUP_H

#include <QWidget>
#include "widgets/qlinkedboundedsliders.h"

namespace Ui {
class QSliderGroup;
}

class QSliderGroup : public QWidget
{
    Q_OBJECT

public:
    explicit QSliderGroup(QWidget *parent = 0);
    ~QSliderGroup();

    void setName(QString name);
    QLinkedBoundedSliders *sliders1();
    QLinkedBoundedSliders *sliders2();

private:
    Ui::QSliderGroup *ui;
};

#endif // QSLIDERGROUP_H
