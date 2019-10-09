/*
 *   Copyright (c) 2019 <xandyx_at_riseup dot net>
 *
 *   This file is part of Radar-App.
 *
 *   Radar-App is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Radar-App is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Radar-App.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <QFont>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
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
    QGuiApplication::setWindowIcon(QIcon(QStringLiteral("qrc:/icons/app-xhdpi.png")));

    auto font = QGuiApplication::font();
    constexpr auto fontSizePt = 16;
    font.setPointSize(fontSizePt);
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
