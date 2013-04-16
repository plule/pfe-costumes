#include <QApplication>
#include <QDebug>
#include <QObject>
#include <QTranslator>
#include <QLocale>
#include <QDir>

#include "mainwindow.h"
 
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    QLocale locale;

    translator.load(QDir(a.applicationDirPath()).filePath("master_fr"));
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
