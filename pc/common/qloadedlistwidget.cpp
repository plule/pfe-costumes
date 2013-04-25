#include "qloadedlistwidget.h"

QLoadedListWidget::QLoadedListWidget(QWidget *parent) :
    QListWidget(parent)
{
    m_loadedItem = 0;
}

void QLoadedListWidget::loadCurrent()
{
    load(currentRow());
}

void QLoadedListWidget::load(int index)
{
    load(this->item(index));
}

void QLoadedListWidget::load(QListWidgetItem *item)
{
    if((row(item) > -1) && item != m_loadedItem) {
        m_loadedItem = item;
        for(int i=0; i<count(); i++) {
            QFont f = this->item(i)->font();
            f.setBold(false);
            this->item(i)->setFont(f);
        }
        QFont f = item->font();
        f.setBold(true);
        item->setFont(f);
        emit loadedChanged(row(item));
        emit loadedChanged(item);
    }
}

void QLoadedListWidget::dirty(int index, bool dirty)
{
    setDirty(item(index),dirty);
}

void QLoadedListWidget::cleanAll()
{
    for(int i=0; i<count(); i++)
        dirty(i,false);
}

QListWidgetItem *QLoadedListWidget::loadedItem() const
{
    return m_loadedItem;
}

void QLoadedListWidget::setLoadedItem(QListWidgetItem *loadedItem)
{
    m_loadedItem = loadedItem;
}

void QLoadedListWidget::setDirty(QListWidgetItem *item, bool dirty)
{
    if(item) {
        QFont f = item->font();
        f.setItalic(dirty);
        item->setFont(f);
    }
}
