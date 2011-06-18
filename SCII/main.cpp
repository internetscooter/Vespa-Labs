#include <QtGui/QApplication>
#include "sciimainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SCIIMainWindow w;
    w.show();

    return a.exec();
}
