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
#pragma once

#include <QDateTime>
#include <QDesktopServices>
#include <QJSEngine>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QObject>
#include <QQmlEngine>
#include <QStateMachine>

#include "database.h"
#include "eventsmodel.h"
#include "locationprovider.h"

namespace AppState
{
    Q_NAMESPACE
    enum class Values {
        StartupCheck,
        NoConnection,
        Permission,
        CountryLoad,
        CountryFilter,
        CitiesLoad,
        CurrentLocationCheck,
        Loading,
        Extraction,
        Filtering,
        Idle
    };
    Q_ENUM_NS(Values)
}// namespace AppState

class LocationProvider;

class App : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isLoaded READ isLoaded NOTIFY isLoadedChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(QString country READ country WRITE setCountry NOTIFY countryChanged)
    Q_PROPERTY(QString city READ city WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(AppState::Values state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QAbstractListModel *eventsModel READ eventsModel NOTIFY eventsModelChanged)
    Q_PROPERTY(bool noEventsFound READ noEventsFound NOTIFY eventsModelChanged)
    Q_PROPERTY(bool isRememberLocationOn READ isRememberLocationOn NOTIFY rememberLocationChanged)

    Q_PROPERTY(QString title READ title NOTIFY currentEventChanged)
    Q_PROPERTY(QString description READ description NOTIFY currentEventChanged)
    Q_PROPERTY(QString dateTime READ dateTime NOTIFY currentEventChanged)
    Q_PROPERTY(QString duration READ duration NOTIFY currentEventChanged)
    Q_PROPERTY(QString category READ category NOTIFY currentEventChanged)
    Q_PROPERTY(QString url READ eventUrl NOTIFY currentEventChanged)
    Q_PROPERTY(QString price READ price NOTIFY currentEventChanged)
    Q_PROPERTY(QString eventCity READ eventCity NOTIFY currentEventChanged)
    Q_PROPERTY(QString eventCountry READ eventCountry NOTIFY currentEventChanged)

    Q_PROPERTY(QString locationName READ locationName NOTIFY currentLocationChanged)
    Q_PROPERTY(QString locationAddress READ locationAddress NOTIFY currentLocationChanged)
    Q_PROPERTY(QString directions READ directions NOTIFY currentLocationChanged)

    Q_PROPERTY(qreal latitude READ latitude NOTIFY currentLocationChanged)
    Q_PROPERTY(qreal longitude READ longitude NOTIFY currentLocationChanged)
    Q_PROPERTY(QStringList countries READ countries NOTIFY countriesChanged)
    Q_PROPERTY(QStringList cities READ cities NOTIFY citiesChanged)

    Q_PROPERTY(int totalFoundEvents READ totalFoundEvents NOTIFY totalFoundEventsChanged)
    Q_PROPERTY(int todayFoundEvents READ todayFoundEvents NOTIFY todayFoundEventsChanged)

    Q_PROPERTY(QString downloadLink MEMBER m_downloadLink CONSTANT)

public:
    explicit App(QObject *parent = nullptr);
    ~App() override;

    AppState::Values state() const;
    void setState(AppState::Values state);
    bool isLoaded() const;
    bool isConnected() const;
    bool isRememberLocationOn() const;

    const QString &country() const;
    const QString &city() const;
    Q_INVOKABLE void reload();
    Q_INVOKABLE void reloadEvents();

    Q_INVOKABLE void selectEvent(int index);
    Q_INVOKABLE void openLink(const QString &link);
    Q_INVOKABLE void setCity(const QString &city);
    Q_INVOKABLE void showLocation();
    Q_INVOKABLE void addToCalendar();

    Q_INVOKABLE void stopUpdatePosition();
    Q_INVOKABLE void startUpdatePosition();
    Q_INVOKABLE void share();
    Q_INVOKABLE void shareApp();

    Q_INVOKABLE void toggleRememberLocation();
    Q_INVOKABLE void cancelOperation();

    QAbstractListModel *eventsModel() const;
    bool noEventsFound() const;

    const QString &title() const;
    const QString &description() const;
    QString dateTime() const;
    QString duration() const;
    const QString &category() const;
    const QString &price() const;
    QString locationName() const;
    QString locationAddress() const;
    QString eventCity() const;
    QString eventCountry() const;
    QString directions() const;
    QString eventUrl() const;

    qreal longitude() const;
    qreal latitude() const;

    QStringList countries() const;
    void setCountry(const QString &country);
    QStringList cities() const;

    int totalFoundEvents() const;
    int todayFoundEvents() const;

    void updateCurrentLocation();

signals:
    void countriesChanged(QPrivateSignal);
    void allCitiesLoaded(QPrivateSignal);
    void citiesChanged(QPrivateSignal);
    void citiesAlreadyLoaded(QPrivateSignal);

    void isLoadedChanged(QPrivateSignal);
    void isConnectedChanged(QPrivateSignal);
    void countryChanged(QPrivateSignal);
    void cityChanged(QPrivateSignal);
    void rememberLocationChanged(QPrivateSignal);
    void stateChanged(QPrivateSignal);

    void reloadRequested(QPrivateSignal);
    void reloadEventsRequested(QPrivateSignal);
    void loadCompleted(QPrivateSignal);
    void loadFailed(QPrivateSignal);

    void countriesAlreadyLoaded(QPrivateSignal);
    void countriesFiltered(QPrivateSignal);
    void eventListReady(QPrivateSignal);
    void eventListFiltered(QPrivateSignal);
    void eventsModelChanged(QPrivateSignal);

    void currentEventChanged(QPrivateSignal);
    void currentLocationChanged(QPrivateSignal);

    void gotPermissions(QPrivateSignal);
    void failedToGetPermissions(QPrivateSignal);

    void noEvents(QPrivateSignal);
    void eventsExist(QPrivateSignal);

    void userCancelled(QPrivateSignal);
    void totalFoundEventsChanged(QPrivateSignal);
    void todayFoundEventsChanged(QPrivateSignal);

private:
    using MemberFunc = void (App::*)();
    void setupFSM();
    QState *addState(AppState::Values stateEnumVal);
    QState *addState(AppState::Values stateEnumVal, std::function< void() > onEnter);
    QState *addState(AppState::Values stateEnumVal, MemberFunc onEnter);
    void doReload();
    void doLoadCountries();
    void doLoadCities();
    void doFilterCountries();
    void doCurrentLocationCheck();
    void doExtract();
    void doFiltering();
    void getPermissions();

    void initTimeRange();
    void forceSetCountry(const QString &country);

    void updateAllCountries(const QStringList &countries);
    void assignIsLoaded(bool loaded);

    QString sharableBody() const;
    void rememberSelectedLocation();
    void clearEventsModel();
    void doSharing(const QString &title, const QString &body);
    Q_DISABLE_COPY(App)

private:
    QNetworkAccessManager *const m_networkAccessManager;
    LocationProvider *const m_locationProvider;
    DB *const m_db;
    Event m_currentEvent{};
    Location m_currentLocation{};
    QVector< Event > m_allEvents;
    EventsModel *const m_eventsModel;
    QStateMachine m_fsm;
    AppState::Values m_state{AppState::Values::StartupCheck};
    bool m_isLoaded{false};
    bool m_isConnected{false};
    bool m_isRememberLocationOn{false};
    QStringList m_allCountries;
    QString m_country;
    QString m_city;
    QMap< QString, QStringList > m_citiesByCountryCode;
    QSet< QString > m_countriesToLoad;
    const QString m_eventsRequestUrlBase = QStringLiteral("https://radar.squat.net/api/1.2/search/events.json");
    const QString m_groupsRequestUrl = QStringLiteral("https://radar.squat.net/api/1.1/search/groups.json?fields[]=uuid&limit=1");
    //QString m_cityRequestUrlBase = QStringLiteral("https://radar.squat.net/api/1.2/search/events.json?fields[]=uuid&limit=1&facets[country][]=%1");
    const QString m_cityRequestUrlBase = QStringLiteral("https://radar.squat.net/api/1.2/search/groups.json?fields[]=uuid&limit=1&facets[country][]=%1");
    //Locations in city: https://radar.squat.net/api/1.2/search/location.json?facets[country][]=DE&facets[locality][]=Berlin&fields[]=directions&fields[]=title&fields[]=address&fields[]=uuid&fields[]=map
    const QString m_downloadLink = QStringLiteral("https://0xacab.org/xandyx/radar-app/raw/master/apk/android-build-debug.apk");

    QJsonObject m_events;
    QJsonObject m_groups;

    QDateTime m_start;
    QDateTime m_end;
};
