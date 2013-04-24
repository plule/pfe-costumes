#include "qloadedlistwidget.h"

QLoadedListWidget::QLoadedListWidget(QWidget *parent) :
    QListWidget(parent)
{
    loadedItem = 0;
    loadedFont.setBold(true);
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
    if((row(item) > -1) && item != loadedItem) {
        loadedItem = item;
        for(int i=0; i<count(); i++) {
            this->item(i)->setFont(normalFont);
        }
        item->setFont(loadedFont);
        emit loadedChanged(row(item));
        emit loadedChanged(item);
    }
}
