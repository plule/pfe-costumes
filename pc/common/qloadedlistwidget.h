#ifndef QLOADEDLISTWIDGET_H
#define QLOADEDLISTWIDGET_H

#include <QListWidget>
#include <QListWidgetItem>
#include <QFont>
#include <QDebug>

class QLoadedListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit QLoadedListWidget(QWidget *parent = 0);
    
    QListWidgetItem *loadedItem() const;
    void setLoadedItem(QListWidgetItem *loadedItem);

signals:
    void loadedChanged(int index);
    void loadedChanged(QListWidgetItem *item);
    
public slots:
    void loadCurrent();
    void load(int index);
    void load(QListWidgetItem *item);

private:
    QListWidgetItem *m_loadedItem;
    QFont m_loadedFont;
    QFont m_normalFont;

    
};

#endif // QLOADEDLISTWIDGET_H
