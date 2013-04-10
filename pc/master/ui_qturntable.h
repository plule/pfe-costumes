/********************************************************************************
** Form generated from reading UI file 'qturntable.ui'
**
** Created: Wed Apr 10 15:43:19 2013
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTURNTABLE_H
#define UI_QTURNTABLE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QTurntable
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *displayer;
    QSlider *slider;

    void setupUi(QWidget *QTurntable)
    {
        if (QTurntable->objectName().isEmpty())
            QTurntable->setObjectName(QString::fromUtf8("QTurntable"));
        QTurntable->resize(702, 471);
        verticalLayout = new QVBoxLayout(QTurntable);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        displayer = new QLabel(QTurntable);
        displayer->setObjectName(QString::fromUtf8("displayer"));
        displayer->setFrameShape(QFrame::Box);
        displayer->setFrameShadow(QFrame::Sunken);
        displayer->setAlignment(Qt::AlignCenter);
        displayer->setMargin(5);

        verticalLayout->addWidget(displayer);

        slider = new QSlider(QTurntable);
        slider->setObjectName(QString::fromUtf8("slider"));
        slider->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(slider);


        retranslateUi(QTurntable);

        QMetaObject::connectSlotsByName(QTurntable);
    } // setupUi

    void retranslateUi(QWidget *QTurntable)
    {
        QTurntable->setWindowTitle(QApplication::translate("QTurntable", "Form", 0, QApplication::UnicodeUTF8));
        displayer->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class QTurntable: public Ui_QTurntable {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTURNTABLE_H
