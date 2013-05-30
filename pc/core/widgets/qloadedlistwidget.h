#ifndef QLOADEDLISTWIDGET_H
#define QLOADEDLISTWIDGET_H

#include <QListWidget>
#include <QListWidgetItem>
#include <QFont>
#include <QDebug>

/**
 * @brief The QLoadedListWidget class is a QListWidget with bolded and italiqued items
 */
class QLoadedListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit QLoadedListWidget(QWidget *parent = 0);
    
    QListWidgetItem *loadedItem() const;
    void setLoadedItem(QListWidgetItem *loadedItem);
    void setDirty(QListWidgetItem *item, bool dirty);

signals:
    void loadedChanged(int index);
    void loadedChanged(QListWidgetItem *item);
    
public slots:
    void loadCurrent();
    void load(int index);
    void load(QListWidgetItem *item);
    void dirty(int index, bool dirty);
    void cleanAll();

private:
    QListWidgetItem *m_loadedItem;
};

#endif // QLOADEDLISTWIDGET_H
