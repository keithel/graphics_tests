#include <QCoreApplication>
#include "rectangledirectrenderer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RectangleDirectRenderer* r = RectangleDirectRenderer::instance(&a);
    r->fillAlternatingRectsQPainter(QColor("purple"), QColor("teal"), QRect(0,0,1280,240), 0, 100);

    return a.exec();
}
