#include <QtWidgets/QApplication>
#include <QDebug>
#include <QObject>
#include <QTranslator>
#include <QLocale>
#include <QDir>
#include <QResource>
#include <QLibraryInfo>

#include "mainwindow.h"
 
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(pics);
    Q_INIT_RESOURCE(translations);

    a.setApplicationName("pfe-costumes");
    a.setOrganizationName("INSA-CNCS-MJCSteFoy");

    QTranslator translator;
    translator.load(":/translations/master");
    a.installTranslator(&translator);

    QTranslator translator_qt;
    translator_qt.load("qt_" + QLocale::system().name(),QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&translator_qt);

    MainWindow w;
    w.show();
    return a.exec();
}
