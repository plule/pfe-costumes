#ifndef QPHOTODISPLAYER_H
#define QPHOTODISPLAYER_H

#include <QLabel>
#include <QPixmap>

class QPhotoDisplayer : public QLabel
{
    Q_OBJECT
public:
    explicit QPhotoDisplayer(QWidget *parent = 0);
    
protected:
    virtual void paintEvent ( QPaintEvent * event );

signals:
    
public slots:
    virtual void setPixmap (const QPixmap &);

private:
    QPixmap *original_pixmap;
};

#endif // QPHOTODISPLAYER_H
