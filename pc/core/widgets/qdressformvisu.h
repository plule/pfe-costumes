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
    void setBustSideSelected(bool s);
    void setWaistFrontSelected(bool s);
    void setWaistSideSelected(bool s);
    void setHipsFrontSelected(bool s);
    void setHipsSideSelected(bool s);

private:
    Ui::QDressFormVisu *ui;
};

#endif // QDRESSFORMVISU_H
