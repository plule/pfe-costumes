#include <QtWidgets/QApplication>
#include <QDebug>
#include <QObject>
#include <QTranslator>
#include <QLocale>
#include <QDir>
#include <QResource>

#include "mainwindow.h"
 
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(pics);
    Q_INIT_RESOURCE(translations);

    a.setApplicationName("pfe-costumes");
    a.setOrganizationName("INSA-CNCS-MJCSteFoy");
    QTranslator translator;
    QLocale locale;

    //qDebug() << QDir(a.applicationDirPath()).filePath("master_fr");
    //translator.load(QDir(a.applicationDirPath()).filePath("master_fr"));
    translator.load(":/translations/master_fr");
    a.installTranslator(&translator);
    /*foreach(QString strlocale, locale.uiLanguages()) {
        if(translator.load(QDir(a.applicationDirPath()).filePath("master_"+strlocale))) {
            a.installTranslator(&translator);
            break;
        }
    }*/

    MainWindow w;
    w.show();
    return a.exec();
}
