#ifndef MANDATORYFIELDGROUP_H
#define MANDATORYFIELDGROUP_H

#include <QObject>
#include <QList>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>
#include <QtWidgets/QWidget>

class MandatoryFieldGroup : public QObject
{
    Q_OBJECT

public:
    MandatoryFieldGroup(QObject *parent)
        : QObject(parent), okButton(0) {}

    void add(QWidget *widget);
    void remove(QWidget *widget);
    void setOkButton(QPushButton *button);

public slots:
    void clear();

private slots:
    void changed();

private:
    QList<QWidget *> widgets;
    QPushButton *okButton;
};

#endif // MANDATORYFIELDGROUP_H
