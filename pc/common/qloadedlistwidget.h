#ifndef QLOADEDLISTWIDGET_H
#define QLOADEDLISTWIDGET_H

#include <QListWidget>
#include <QListWidgetItem>
#include <QFont>

class QLoadedListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit QLoadedListWidget(QWidget *parent = 0);
    
signals:
    void loadedChanged(int index);
    void loadedChanged(QListWidgetItem item);
    
public slots:
    void loadCurrent();
    void load(int index);
    void load(QListWidgetItem *item);

private:
    QListWidgetItem *loadedItem;
    QFont loadedFont;
    QFont normalFont;

    
};

#endif // QLOADEDLISTWIDGET_H
