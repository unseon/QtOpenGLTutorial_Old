#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <rendersurface.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<RenderSurface>("OpenGLTutorial", 1, 0, "RenderSurface");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
