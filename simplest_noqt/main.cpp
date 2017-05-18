#include <QCoreApplication>
#include "rectangledirectrenderer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RectangleDirectRenderer* r = new RectangleDirectRenderer(&a);

    return a.exec();
}
