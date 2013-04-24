#include "qloadedlistwidget.h"

QLoadedListWidget::QLoadedListWidget(QWidget *parent) :
    QListWidget(parent)
{
    m_loadedItem = 0;
    m_loadedFont.setBold(true);
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
            this->item(i)->setFont(m_normalFont);
        }
        item->setFont(m_loadedFont);
        emit loadedChanged(row(item));
        emit loadedChanged(item);
    }
}

QListWidgetItem *QLoadedListWidget::loadedItem() const
{
    return m_loadedItem;
}

void QLoadedListWidget::setLoadedItem(QListWidgetItem *loadedItem)
{
    m_loadedItem = loadedItem;
}
