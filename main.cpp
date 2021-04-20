#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Flooble");
    QCoreApplication::setOrganizationDomain("unknown");
    QCoreApplication::setApplicationName("Cock Heroine");
    MainWindow w;
    w.setWindowIcon(QIcon(":/WandIcon"));
    w.setWindowTitle(QString("Cock Heroine"));
    w.show();

    return a.exec();
}
