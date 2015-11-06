#ifndef QDRESSFORMVISU_H
#define QDRESSFORMVISU_H

#include <QWidget>

namespace Ui {
class QDressFormVisu;
}

class QDressFormVisu : public QWidget
{
    Q_OBJECT

public:
    explicit QDressFormVisu(QWidget *parent = 0);
    ~QDressFormVisu();

public slots:
    void setBustFrontSelected(bool s);
    void setBustBackSelected(bool s);
    void setBustLeftSelected(bool s);
    void setBustRightSelected(bool s);
    void setWaistFrontSelected(bool s);
    void setWaistBackSelected(bool s);
    void setWaistLeftSelected(bool s);
    void setWaistRightSelected(bool s);
    void setHipsFrontSelected(bool s);
    void setHipsBackSelected(bool s);
    void setHipsLeftSelected(bool s);
    void setHipsRightSelected(bool s);

private:
    Ui::QDressFormVisu *ui;
};

#endif // QDRESSFORMVISU_H
