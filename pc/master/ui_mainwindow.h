/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Wed Apr 10 16:18:07 2013
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDial>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *mainLayout;
    QVBoxLayout *picLayout;
    QVBoxLayout *controlLayout;
    QPushButton *refreshButton;
    QGroupBox *massCaptureBox;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton;
    QHBoxLayout *horizontalLayout;
    QLabel *fromLabel;
    QSpinBox *fromInput;
    QLabel *toLabel;
    QSpinBox *toInput;
    QGroupBox *singleCaptureGroup;
    QVBoxLayout *verticalLayout_3;
    QPushButton *captureButton;
    QGroupBox *viewGroup;
    QVBoxLayout *verticalLayout_4;
    QDial *dial;
    QHBoxLayout *horizontalLayout_2;
    QSpinBox *spinBox;
    QLabel *label;
    QGroupBox *controlGroup;
    QVBoxLayout *verticalLayout_5;
    QPushButton *viewToModelButton;
    QCheckBox *autoRotateCheckBox;
    QSpacerItem *verticalSpacer;
    QProgressBar *workBar;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(891, 665);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        mainLayout = new QHBoxLayout();
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        picLayout = new QVBoxLayout();
        picLayout->setObjectName(QString::fromUtf8("picLayout"));
        picLayout->setSizeConstraint(QLayout::SetMaximumSize);

        mainLayout->addLayout(picLayout);

        controlLayout = new QVBoxLayout();
        controlLayout->setObjectName(QString::fromUtf8("controlLayout"));
        controlLayout->setSizeConstraint(QLayout::SetMinimumSize);
        refreshButton = new QPushButton(centralwidget);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));
        refreshButton->setEnabled(true);

        controlLayout->addWidget(refreshButton);

        massCaptureBox = new QGroupBox(centralwidget);
        massCaptureBox->setObjectName(QString::fromUtf8("massCaptureBox"));
        verticalLayout = new QVBoxLayout(massCaptureBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        pushButton = new QPushButton(massCaptureBox);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        verticalLayout->addWidget(pushButton);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        fromLabel = new QLabel(massCaptureBox);
        fromLabel->setObjectName(QString::fromUtf8("fromLabel"));
        fromLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(fromLabel);

        fromInput = new QSpinBox(massCaptureBox);
        fromInput->setObjectName(QString::fromUtf8("fromInput"));

        horizontalLayout->addWidget(fromInput);

        toLabel = new QLabel(massCaptureBox);
        toLabel->setObjectName(QString::fromUtf8("toLabel"));
        toLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(toLabel);

        toInput = new QSpinBox(massCaptureBox);
        toInput->setObjectName(QString::fromUtf8("toInput"));

        horizontalLayout->addWidget(toInput);


        verticalLayout->addLayout(horizontalLayout);


        controlLayout->addWidget(massCaptureBox);

        singleCaptureGroup = new QGroupBox(centralwidget);
        singleCaptureGroup->setObjectName(QString::fromUtf8("singleCaptureGroup"));
        verticalLayout_3 = new QVBoxLayout(singleCaptureGroup);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        captureButton = new QPushButton(singleCaptureGroup);
        captureButton->setObjectName(QString::fromUtf8("captureButton"));

        verticalLayout_3->addWidget(captureButton);


        controlLayout->addWidget(singleCaptureGroup);

        viewGroup = new QGroupBox(centralwidget);
        viewGroup->setObjectName(QString::fromUtf8("viewGroup"));
        verticalLayout_4 = new QVBoxLayout(viewGroup);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        dial = new QDial(viewGroup);
        dial->setObjectName(QString::fromUtf8("dial"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dial->sizePolicy().hasHeightForWidth());
        dial->setSizePolicy(sizePolicy);
        dial->setMinimumSize(QSize(50, 50));
        dial->setOrientation(Qt::Horizontal);
        dial->setWrapping(true);
        dial->setNotchTarget(2);
        dial->setNotchesVisible(true);

        verticalLayout_4->addWidget(dial);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        spinBox = new QSpinBox(viewGroup);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));

        horizontalLayout_2->addWidget(spinBox);

        label = new QLabel(viewGroup);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_2->addWidget(label);


        verticalLayout_4->addLayout(horizontalLayout_2);


        controlLayout->addWidget(viewGroup);

        controlGroup = new QGroupBox(centralwidget);
        controlGroup->setObjectName(QString::fromUtf8("controlGroup"));
        verticalLayout_5 = new QVBoxLayout(controlGroup);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        viewToModelButton = new QPushButton(controlGroup);
        viewToModelButton->setObjectName(QString::fromUtf8("viewToModelButton"));

        verticalLayout_5->addWidget(viewToModelButton);

        autoRotateCheckBox = new QCheckBox(controlGroup);
        autoRotateCheckBox->setObjectName(QString::fromUtf8("autoRotateCheckBox"));

        verticalLayout_5->addWidget(autoRotateCheckBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer);


        controlLayout->addWidget(controlGroup);


        mainLayout->addLayout(controlLayout);


        verticalLayout_2->addLayout(mainLayout);

        workBar = new QProgressBar(centralwidget);
        workBar->setObjectName(QString::fromUtf8("workBar"));
        workBar->setValue(0);

        verticalLayout_2->addWidget(workBar);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 891, 27));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        refreshButton->setText(QApplication::translate("MainWindow", "Rafra\303\256chir", 0, QApplication::UnicodeUTF8));
        massCaptureBox->setTitle(QApplication::translate("MainWindow", "Captures multiples", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainWindow", "Lancer les captures", 0, QApplication::UnicodeUTF8));
        fromLabel->setText(QApplication::translate("MainWindow", "De : ", 0, QApplication::UnicodeUTF8));
        toLabel->setText(QApplication::translate("MainWindow", "\303\200 : ", 0, QApplication::UnicodeUTF8));
        singleCaptureGroup->setTitle(QApplication::translate("MainWindow", "Capture unique", 0, QApplication::UnicodeUTF8));
        captureButton->setText(QApplication::translate("MainWindow", "(Re)capturer la vue actuelle", 0, QApplication::UnicodeUTF8));
        viewGroup->setTitle(QApplication::translate("MainWindow", "Vue", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "(y\302\260)", 0, QApplication::UnicodeUTF8));
        controlGroup->setTitle(QApplication::translate("MainWindow", "Contr\303\264le", 0, QApplication::UnicodeUTF8));
        viewToModelButton->setText(QApplication::translate("MainWindow", "Appliquer rotation", 0, QApplication::UnicodeUTF8));
        autoRotateCheckBox->setText(QApplication::translate("MainWindow", "Tourner automatiquement avant prise de vue", 0, QApplication::UnicodeUTF8));
        workBar->setFormat(QString());
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
