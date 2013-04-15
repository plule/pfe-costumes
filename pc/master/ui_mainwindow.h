/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Mon Apr 15 17:34:49 2013
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
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <qturntable.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QTabWidget *tabWidget;
    QWidget *preparationTab;
    QWidget *adjustmentTab;
    QWidget *CaptureTab;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *mainLayout;
    QVBoxLayout *picLayout;
    QTurntable *turntable;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *fitButton;
    QPushButton *resetScaleButton;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *controlLayout;
    QPushButton *refreshButton;
    QPushButton *suzanneButton;
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
    QCheckBox *autofocusCheckBox;
    QGroupBox *viewGroup;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *angleLayout;
    QDial *dial;
    QSpinBox *angleBox;
    QVBoxLayout *zoomLayout;
    QSlider *zoomSlider;
    QSpinBox *zoomBox;
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
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        preparationTab = new QWidget();
        preparationTab->setObjectName(QString::fromUtf8("preparationTab"));
        tabWidget->addTab(preparationTab, QString());
        adjustmentTab = new QWidget();
        adjustmentTab->setObjectName(QString::fromUtf8("adjustmentTab"));
        tabWidget->addTab(adjustmentTab, QString());
        CaptureTab = new QWidget();
        CaptureTab->setObjectName(QString::fromUtf8("CaptureTab"));
        verticalLayout_6 = new QVBoxLayout(CaptureTab);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        mainLayout = new QHBoxLayout();
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        picLayout = new QVBoxLayout();
        picLayout->setObjectName(QString::fromUtf8("picLayout"));
        picLayout->setSizeConstraint(QLayout::SetMaximumSize);
        turntable = new QTurntable(CaptureTab);
        turntable->setObjectName(QString::fromUtf8("turntable"));
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        turntable->setBackgroundBrush(brush);
        turntable->setDragMode(QGraphicsView::ScrollHandDrag);
        turntable->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        picLayout->addWidget(turntable);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        fitButton = new QPushButton(CaptureTab);
        fitButton->setObjectName(QString::fromUtf8("fitButton"));

        horizontalLayout_3->addWidget(fitButton);

        resetScaleButton = new QPushButton(CaptureTab);
        resetScaleButton->setObjectName(QString::fromUtf8("resetScaleButton"));

        horizontalLayout_3->addWidget(resetScaleButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        picLayout->addLayout(horizontalLayout_3);


        mainLayout->addLayout(picLayout);

        controlLayout = new QVBoxLayout();
        controlLayout->setObjectName(QString::fromUtf8("controlLayout"));
        controlLayout->setSizeConstraint(QLayout::SetMinimumSize);
        refreshButton = new QPushButton(CaptureTab);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));
        refreshButton->setEnabled(true);

        controlLayout->addWidget(refreshButton);

        suzanneButton = new QPushButton(CaptureTab);
        suzanneButton->setObjectName(QString::fromUtf8("suzanneButton"));

        controlLayout->addWidget(suzanneButton);

        massCaptureBox = new QGroupBox(CaptureTab);
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
        fromInput->setMaximum(360);

        horizontalLayout->addWidget(fromInput);

        toLabel = new QLabel(massCaptureBox);
        toLabel->setObjectName(QString::fromUtf8("toLabel"));
        toLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(toLabel);

        toInput = new QSpinBox(massCaptureBox);
        toInput->setObjectName(QString::fromUtf8("toInput"));
        toInput->setMaximum(360);
        toInput->setValue(360);

        horizontalLayout->addWidget(toInput);


        verticalLayout->addLayout(horizontalLayout);


        controlLayout->addWidget(massCaptureBox);

        singleCaptureGroup = new QGroupBox(CaptureTab);
        singleCaptureGroup->setObjectName(QString::fromUtf8("singleCaptureGroup"));
        verticalLayout_3 = new QVBoxLayout(singleCaptureGroup);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        captureButton = new QPushButton(singleCaptureGroup);
        captureButton->setObjectName(QString::fromUtf8("captureButton"));

        verticalLayout_3->addWidget(captureButton);

        autofocusCheckBox = new QCheckBox(singleCaptureGroup);
        autofocusCheckBox->setObjectName(QString::fromUtf8("autofocusCheckBox"));

        verticalLayout_3->addWidget(autofocusCheckBox);


        controlLayout->addWidget(singleCaptureGroup);

        viewGroup = new QGroupBox(CaptureTab);
        viewGroup->setObjectName(QString::fromUtf8("viewGroup"));
        horizontalLayout_5 = new QHBoxLayout(viewGroup);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        angleLayout = new QVBoxLayout();
        angleLayout->setObjectName(QString::fromUtf8("angleLayout"));
        dial = new QDial(viewGroup);
        dial->setObjectName(QString::fromUtf8("dial"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dial->sizePolicy().hasHeightForWidth());
        dial->setSizePolicy(sizePolicy);
        dial->setMinimumSize(QSize(50, 50));
        dial->setMinimum(0);
        dial->setMaximum(359);
        dial->setOrientation(Qt::Horizontal);
        dial->setWrapping(true);
        dial->setNotchTarget(10);
        dial->setNotchesVisible(true);

        angleLayout->addWidget(dial);

        angleBox = new QSpinBox(viewGroup);
        angleBox->setObjectName(QString::fromUtf8("angleBox"));
        angleBox->setWrapping(true);
        angleBox->setReadOnly(false);
        angleBox->setMinimum(0);
        angleBox->setMaximum(359);

        angleLayout->addWidget(angleBox);


        horizontalLayout_5->addLayout(angleLayout);

        zoomLayout = new QVBoxLayout();
        zoomLayout->setObjectName(QString::fromUtf8("zoomLayout"));
        zoomSlider = new QSlider(viewGroup);
        zoomSlider->setObjectName(QString::fromUtf8("zoomSlider"));
        zoomSlider->setMinimum(1);
        zoomSlider->setMaximum(200);
        zoomSlider->setOrientation(Qt::Vertical);

        zoomLayout->addWidget(zoomSlider);

        zoomBox = new QSpinBox(viewGroup);
        zoomBox->setObjectName(QString::fromUtf8("zoomBox"));
        zoomBox->setMinimum(1);
        zoomBox->setMaximum(200);

        zoomLayout->addWidget(zoomBox);


        horizontalLayout_5->addLayout(zoomLayout);


        controlLayout->addWidget(viewGroup);

        controlGroup = new QGroupBox(CaptureTab);
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


        verticalLayout_6->addLayout(mainLayout);

        workBar = new QProgressBar(CaptureTab);
        workBar->setObjectName(QString::fromUtf8("workBar"));
        workBar->setValue(0);

        verticalLayout_6->addWidget(workBar);

        tabWidget->addTab(CaptureTab, QString());

        verticalLayout_2->addWidget(tabWidget);

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
#ifndef QT_NO_SHORTCUT
        fromLabel->setBuddy(fromInput);
        toLabel->setBuddy(toInput);
#endif // QT_NO_SHORTCUT

        retranslateUi(MainWindow);
        QObject::connect(fitButton, SIGNAL(pressed()), turntable, SLOT(fitInView()));
        QObject::connect(resetScaleButton, SIGNAL(pressed()), turntable, SLOT(resetScale()));
        QObject::connect(dial, SIGNAL(valueChanged(int)), turntable, SLOT(setAngle(int)));
        QObject::connect(angleBox, SIGNAL(valueChanged(int)), turntable, SLOT(setAngle(int)));
        QObject::connect(turntable, SIGNAL(angleChanged(int)), dial, SLOT(setValue(int)));
        QObject::connect(turntable, SIGNAL(angleChanged(int)), angleBox, SLOT(setValue(int)));
        QObject::connect(zoomSlider, SIGNAL(valueChanged(int)), turntable, SLOT(setZoom(int)));
        QObject::connect(zoomBox, SIGNAL(valueChanged(int)), turntable, SLOT(setZoom(int)));
        QObject::connect(turntable, SIGNAL(zoomChanged(int)), zoomSlider, SLOT(setValue(int)));
        QObject::connect(turntable, SIGNAL(zoomChanged(int)), zoomBox, SLOT(setValue(int)));

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(preparationTab), QApplication::translate("MainWindow", "Pr\303\251paration", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(adjustmentTab), QApplication::translate("MainWindow", "Ajustement", 0, QApplication::UnicodeUTF8));
        fitButton->setText(QApplication::translate("MainWindow", "Ajuster \303\240 la fen\303\252tre", 0, QApplication::UnicodeUTF8));
        resetScaleButton->setText(QApplication::translate("MainWindow", "1:1", 0, QApplication::UnicodeUTF8));
        refreshButton->setText(QApplication::translate("MainWindow", "Rafra\303\256chir", 0, QApplication::UnicodeUTF8));
        suzanneButton->setText(QApplication::translate("MainWindow", "Suzanne", 0, QApplication::UnicodeUTF8));
        massCaptureBox->setTitle(QApplication::translate("MainWindow", "Captures multiples", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainWindow", "Lancer les captures", 0, QApplication::UnicodeUTF8));
        fromLabel->setText(QApplication::translate("MainWindow", "De : ", 0, QApplication::UnicodeUTF8));
        fromInput->setSuffix(QApplication::translate("MainWindow", "\302\260", 0, QApplication::UnicodeUTF8));
        toLabel->setText(QApplication::translate("MainWindow", "\303\200 : ", 0, QApplication::UnicodeUTF8));
        toInput->setSuffix(QApplication::translate("MainWindow", "\302\260", 0, QApplication::UnicodeUTF8));
        singleCaptureGroup->setTitle(QApplication::translate("MainWindow", "Capture unique", 0, QApplication::UnicodeUTF8));
        captureButton->setText(QApplication::translate("MainWindow", "(Re)capturer la vue actuelle", 0, QApplication::UnicodeUTF8));
        autofocusCheckBox->setText(QApplication::translate("MainWindow", "Autofocus", 0, QApplication::UnicodeUTF8));
        viewGroup->setTitle(QApplication::translate("MainWindow", "Vue", 0, QApplication::UnicodeUTF8));
        angleBox->setSuffix(QApplication::translate("MainWindow", "\302\260", 0, QApplication::UnicodeUTF8));
        zoomBox->setSuffix(QApplication::translate("MainWindow", "%", 0, QApplication::UnicodeUTF8));
        controlGroup->setTitle(QApplication::translate("MainWindow", "Contr\303\264le", 0, QApplication::UnicodeUTF8));
        viewToModelButton->setText(QApplication::translate("MainWindow", "Appliquer rotation", 0, QApplication::UnicodeUTF8));
        autoRotateCheckBox->setText(QApplication::translate("MainWindow", "Tourner automatiquement", 0, QApplication::UnicodeUTF8));
        workBar->setFormat(QString());
        tabWidget->setTabText(tabWidget->indexOf(CaptureTab), QApplication::translate("MainWindow", "Capture", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
