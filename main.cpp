#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFont>
#include <QQmlContext>
#include <QtSql>

#include "app.h"

static QObject *createAppInstance(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)
    App *instance = new App(qmlEngine);
    return instance;
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("UnknownDeveloper"));
    app.setOrganizationDomain(QStringLiteral("unknownsoft.org"));
    app.setApplicationName(QStringLiteral("Radar-App"));
    app.setApplicationDisplayName(QStringLiteral("Radar Application"));

    auto font = app.font();
    font.setPointSize(16);
    app.setFont(font);

    qRegisterMetaType< QAbstractItemModel * >();
    qmlRegisterSingletonType< App >("org.radar.app", 1, 0, "App", createAppInstance);
    qmlRegisterUncreatableMetaObject(AppState::staticMetaObject, "org.radar.app", 1, 0, "AppStates", "Error: only enums!");
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.rootContext()->setContextProperty(QStringLiteral("fontPointSize"), app.font().pointSizeF());
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
                     [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl) {
                             QCoreApplication::exit(-1);
                         }
                     },
                     Qt::QueuedConnection);
    engine.load(url);
    return app.exec();
}
