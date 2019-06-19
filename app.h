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
    enum class Values { StartupCheck, NoConnection, Permission, Loading, Extraction, Filtering, Idle };
    Q_ENUM_NS(Values)
}// namespace AppState

class LocationProvider;

class App : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isLoaded READ isLoaded NOTIFY isLoadedChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(QString country READ country NOTIFY countryChanged)
    Q_PROPERTY(QString city READ city WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(AppState::Values state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QAbstractItemModel *eventsModel READ eventsModel NOTIFY eventsModelChanged)

    Q_PROPERTY(QString title READ title NOTIFY currentEventChanged)
    Q_PROPERTY(QString description READ description NOTIFY currentEventChanged)
    Q_PROPERTY(QString dateTime READ dateTime NOTIFY currentEventChanged)
    Q_PROPERTY(QString category READ category NOTIFY currentEventChanged)
    Q_PROPERTY(QString price READ price NOTIFY currentEventChanged)
    Q_PROPERTY(QString eventCity READ eventCity NOTIFY currentEventChanged)
    Q_PROPERTY(QString eventCountry READ eventCountry NOTIFY currentEventChanged)

    Q_PROPERTY(QString locationName READ locationName NOTIFY currentLocationChanged)
    Q_PROPERTY(QString locationAddress READ locationAddress NOTIFY currentLocationChanged)
    Q_PROPERTY(QString directions READ directions NOTIFY currentLocationChanged)

public:
    explicit App(QObject *parent = nullptr);
    ~App();

    AppState::Values state() const;
    void setState(AppState::Values state);
    bool isLoaded() const;
    bool isConnected() const;
    const QString &country() const;
    const QString &city() const;
    Q_INVOKABLE void reload();
    Q_INVOKABLE void selectEvent(int index);
    Q_INVOKABLE void openLink(const QString &link);
    Q_INVOKABLE void setCity(const QString &city);
    Q_INVOKABLE void showLocation(const QString &location);
    EventsModel *eventsModel() const;

    const QString &title() const;
    const QString &description() const;
    const QString dateTime() const;
    const QString &category() const;
    const QString &price() const;
    QString locationName() const;
    QString locationAddress() const;
    const QString &eventCity() const;
    const QString &eventCountry() const;
    QString directions() const;

signals:
    void isLoadedChanged(QPrivateSignal);
    void isConnectedChanged(QPrivateSignal);
    void countryChanged(QPrivateSignal);
    void cityChanged(QPrivateSignal);
    void stateChanged(QPrivateSignal);

    void reloadRequested(QPrivateSignal);
    void loadCompleted(QPrivateSignal);
    void loadFailed(QPrivateSignal);

    void eventListReady(QPrivateSignal);
    void eventListFiltered(QPrivateSignal);
    void eventsModelChanged(QPrivateSignal);

    void currentEventChanged(QPrivateSignal);
    void currentLocationChanged(QPrivateSignal);

    void gotPermissions(QPrivateSignal);
    void failedToGetPermissions(QPrivateSignal);

private:
    using MemberFunc = void (App::*)();
    void setupFSM();
    QState *addState(AppState::Values stateEnumVal);
    QState *addState(AppState::Values stateEnumVal, std::function< void() > onEnter);
    QState *addState(AppState::Values stateEnumVal, MemberFunc onEnter);
    void doReload();
    void doExtract();
    void doFiltering();
    void getPermissions();

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
    QString m_country;
    QString m_city;
    QString m_eventsRequestUrlBase = QStringLiteral("https://radar.squat.net/api/1.2/search/events.json");
    QJsonObject m_events;

    QDateTime m_start;
    QDateTime m_end;

    Q_DISABLE_COPY(App)
    void initTimeRange();
};
