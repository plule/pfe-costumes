#include <QtCore/QCoreApplication>
#include <QDebug>
 
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "hello world!" << endl;
    return a.exec();
}
