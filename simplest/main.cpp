#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "painttimelogger.h"

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    qmlRegisterType<PaintTimeLogger>("com.l3", 1, 0, "PaintTimeLogger");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
