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
    QGuiApplication::setOrganizationName(QStringLiteral("UnknownDeveloper"));
    QGuiApplication::setOrganizationDomain(QStringLiteral("unknownsoft.org"));
    QGuiApplication::setApplicationName(QStringLiteral("Radar-App"));
    QGuiApplication::setApplicationDisplayName(QStringLiteral("Radar Application"));

    auto font = QGuiApplication::font();
    font.setPointSize(16);
    QGuiApplication::setFont(font);

    qRegisterMetaType< QAbstractItemModel * >();
    qmlRegisterSingletonType< App >("org.radar.app", 1, 0, "App", createAppInstance);
    qmlRegisterUncreatableMetaObject(AppState::staticMetaObject, "org.radar.app", 1, 0, "AppStates", "Error: only enums!");
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.rootContext()->setContextProperty(QStringLiteral("fontPointSize"), QGuiApplication::font().pointSizeF());
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
                     [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl) {
                             QCoreApplication::exit(-1);
                         }
                     },
                     Qt::QueuedConnection);
    engine.load(url);
    return QGuiApplication::exec();
}
