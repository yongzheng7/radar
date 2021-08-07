/*
 *   Copyright (c) 2021 <xandyx_at_riseup dot net>
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
#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#include <QtQuickControls2>
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
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationName(QStringLiteral("UnknownDeveloper"));
    QGuiApplication::setOrganizationDomain(QStringLiteral("unknownsoft.org"));
    QGuiApplication::setApplicationName(QStringLiteral("Radar App"));
    QGuiApplication::setApplicationDisplayName(QStringLiteral("Radar Application"));
    QGuiApplication::setWindowIcon(QIcon(QStringLiteral("qrc:/icons/app-xhdpi.png")));

    QIcon::setThemeName(QStringLiteral("radar"));

    {
        QTranslator *tranlator = new QTranslator(&app);
        qDebug() << "Locale:" << QLocale().system().name() << QLocale().system().language();
        if (!tranlator->load(QStringLiteral(":/radar.qm"))) {
            qCritical() << "Failed to load translator!";
            return 1;
        }
        app.installTranslator(tranlator);
    }

    {
        auto font = QGuiApplication::font();
        auto fontSizePt = 16;
        fontSizePt *= App::getAndroidScale();
        font.setPointSize(fontSizePt);
        QGuiApplication::setFont(font);
    }

    {
        int fontId = QFontDatabase::addApplicationFont(":/fonts/materialdesignicons-webfont.ttf");
        Q_ASSERT(fontId!= -1);
        qDebug() << "font id=" << fontId;
        qDebug() << QFontDatabase::applicationFontFamilies(fontId);
    }


    qRegisterMetaType< QAbstractItemModel * >();
    qmlRegisterSingletonType< App >("org.radar.app", 1, 0, "App", createAppInstance);
    qmlRegisterUncreatableMetaObject(AppState::staticMetaObject, "org.radar.app", 1, 0, "AppStates", "Error: only enums!");

    QQuickStyle::setStyle(QStringLiteral("Material"));
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/ui/main.qml"));
    engine.rootContext()->setContextProperty(QStringLiteral("fontPointSize"), QGuiApplication::font().pointSizeF());
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
                     [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl) {
                             QCoreApplication::exit(-1);
                         }
                     },
                     Qt::QueuedConnection);
    QObject::connect(&engine, &QQmlApplicationEngine::warnings, &engine, [](const QList< QQmlError > &warnings) noexcept {
        for (const auto &warning : warnings) {
            qCritical() << "QML Warning:" << warning.url() << ":" << warning.line() << warning.toString();
            if (warning.toString().endsWith(QStringLiteral("svg"))) {
                return;
            }
        }
        if (!warnings.isEmpty()) {
            Q_ASSERT(false);
        }
    });
    engine.load(url);
    return QGuiApplication::exec();
}
