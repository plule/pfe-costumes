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
    int loadedIndex = row(loadedItem);
    if(index != loadedIndex && item(index)) {
        loadedItem = item(index);
        for(int i=0; i<count(); i++) {
            item(i)->setFont(normalFont);
        }
        loadedItem->setFont(loadedFont);
        emit loadedChanged(index);
        emit loadedChanged(loadedItem);
    }
}

void QLoadedListWidget::load(QListWidgetItem *item)
{
    load(row(item));
}
