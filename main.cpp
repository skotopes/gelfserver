#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("Plooks Ltd");
    a.setOrganizationDomain("plooks.com");
    a.setApplicationName("GELF Server");
    a.setWindowIcon(QIcon(":/GELFServer.svg"));

    MainWindow w;
    w.show();

    return a.exec();
}
