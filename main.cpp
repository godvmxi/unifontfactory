#include <QtGui/QApplication>
#include "unifontfactory.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UniFontFactory w;
    w.resize(640, 480);
    w.show();
    return a.exec();
}
