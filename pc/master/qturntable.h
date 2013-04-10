#ifndef QTURNTABLE_H
#define QTURNTABLE_H

#include <QWidget>
#include <QList>
#include <QDebug>

namespace Ui {
class QTurntable;
}

class QTurntable : public QWidget
{
    Q_OBJECT
    
public:
    explicit QTurntable(QWidget *parent = 0);
    ~QTurntable();

public slots:
    virtual void addPixmap (const QPixmap &);

protected:
    virtual void paintEvent ( QPaintEvent * event );
    
private slots:
    void on_slider_sliderMoved(int position);

private:
    Ui::QTurntable *ui;
    QList<QPixmap> *pixmaps;
    int currentPixmap;
};

#endif // QTURNTABLE_H
