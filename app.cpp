/*
 *   Copyright (c) 2019-2020 <xandyx_at_riseup dot net>
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
#include "app.h"

#include "all_countries.h"
#include "eventsmodel.h"
#include "locationprovider.h"

#include <algorithm>

#include <QtDebug>

#include <QJsonObject>
#include <QTextDocument>
#include <QUrl>
#include <QtNetwork>

#include <QtGlobal>
#include <chrono>
#include <utility>

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QtAndroid>
#include <QtAndroidExtras/QAndroidIntent>
#endif

namespace
{
    const QString settingsCityKey{QStringLiteral("filter/city")};
    const QString settingsCountryKey{QStringLiteral("filter/country")};
}// namespace
App::App(QObject *parent)
    : QObject(parent)
    , m_networkAccessManager(new QNetworkAccessManager(this))
    , m_locationProvider(new LocationProvider(this))
    , m_db(new DB(this))
    , m_eventsModel(new EventsModel(m_locationProvider, this))
    , m_allCountries(Countries::allCountries().keys())
{
    m_allCountries.sort();
    qRegisterMetaType< Event >();
    qRegisterMetaType< Location >();
    auto configuration = m_networkAccessManager->configuration();
    constexpr auto timeout = 30;
    configuration.setConnectTimeout(
        std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::seconds(timeout)).count());
    m_networkAccessManager->setConfiguration(configuration);
    // m_locationProvider->setNetworkAccessManager(m_networkAccessManager);
    QSqlError err = m_db->initDB();
    if (err.type() != QSqlError::NoError) {
        qCritical() << err.text();
        qFatal("Failed to init DB!");
    }
    m_locationProvider->setDB(m_db);

    initTimeRange();
    QSettings settings;

    m_city = settings.value(settingsCityKey, QStringLiteral("Berlin")).toString();
    m_country = settings.value(settingsCountryKey, QStringLiteral("Germany")).toString();

    qDebug() << __FUNCTION__;
    setupFSM();
    m_fsm.start();
    connect(m_locationProvider, &LocationProvider::locationAvailable,
            this, [this](const QUuid &id, const Location &location) noexcept {
                if (m_currentEvent.locationID != id) {
                    return;
                }
                qDebug() << "Location loaded for " << id;
                qDebug() << "Location:" << location.toString();
                m_currentLocation = location;
                emit this->currentLocationChanged(QPrivateSignal());
            });

    connect(m_networkAccessManager, &QNetworkAccessManager::networkAccessibleChanged,
            this, [this](const QNetworkAccessManager::NetworkAccessibility accessible) noexcept {
                qDebug() << "m_networkAccessManager = " << accessible;
                if (!isLoaded() && accessible == QNetworkAccessManager::NetworkAccessibility::Accessible) {
                    emit this->reloadEventsRequested(QPrivateSignal());
                }
            });
    setCountry(m_country);
}

App::~App() = default;

void App::setupFSM()
{
    auto idle = addState(AppState::Values::Idle);
    auto countryLoad = addState(AppState::Values::CountryLoad, &App::doLoadCountries);
    auto countryFilter = addState(AppState::Values::CountryFilter, &App::doFilterCountries);
    auto citiesLoad = addState(AppState::Values::CitiesLoad, &App::doLoadCities);
    auto checkCurrentLocation = addState(AppState::Values::CurrentLocationCheck, &App::doCurrentLocationCheck);
    auto loading = addState(AppState::Values::Loading, &App::doReload);
    auto extraction = addState(AppState::Values::Extraction, &App::doExtract);
    auto filtering = addState(AppState::Values::Filtering, &App::doFiltering);
#ifdef Q_OS_ANDROID
    auto requestingPermissions = addState(AppState::Values::Permission, &App::getPermissions);
    requestingPermissions->addTransition(this, &App::gotPermissions, countryLoad);
    requestingPermissions->addTransition(this, &App::failedToGetPermissions, idle);
#endif
    m_fsm.setInitialState(idle);

    connect(&m_fsm, &QStateMachine::runningChanged, this, [](bool running) { qDebug() << "runningChanged to " << running; });

#ifndef Q_OS_ANDROID
    idle->addTransition(this, &App::reloadRequested, countryLoad);
#else
    idle->addTransition(this, &App::reloadRequested, requestingPermissions);
#endif
    countryLoad->addTransition(this, &App::countriesAlreadyLoaded, citiesLoad);
    countryLoad->addTransition(this, &App::loadCompleted, countryFilter);
    countryFilter->addTransition(this, &App::countriesFiltered, citiesLoad);
    citiesLoad->addTransition(this, &App::citiesAlreadyLoaded, checkCurrentLocation);
    checkCurrentLocation->addTransition(this, &App::noEvents, loading);
    checkCurrentLocation->addTransition(this, &App::eventsExist, idle);
    auto transition = citiesLoad->addTransition(this, &App::allCitiesLoaded, idle);
    connect(transition, &QAbstractTransition::triggered, this, &App::updateCurrentLocation);
    idle->addTransition(this, &App::reloadEventsRequested, loading);
    loading->addTransition(this, &App::userCancelled, idle);
    loading->addTransition(this, &App::loadCompleted, extraction);
    loading->addTransition(this, &App::loadFailed, idle);// FIXME
    extraction->addTransition(this, &App::eventListReady, filtering);
    extraction->addTransition(this, &App::userCancelled, idle);

    filtering->addTransition(this, &App::userCancelled, idle);
    auto getEventsFinished = filtering->addTransition(this, &App::eventListFiltered, idle);
    connect(getEventsFinished, &QAbstractTransition::triggered, this, [this]() noexcept {
        assignIsLoaded(true);
        if (isRememberLocationOn()) {
            rememberSelectedLocation();
        }
    });
}

QState *App::addState(AppState::Values stateEnumVal)
{
    auto state = new QState(&m_fsm);
    state->assignProperty(this, "state", QVariant::fromValue< AppState::Values >(stateEnumVal));
    connect(state, &QAbstractState::entered, this, [stateEnumVal]() noexcept {
        qDebug() << "FSM State:" << QMetaEnum::fromType< AppState::Values >().valueToKey(static_cast< int >(stateEnumVal));
    });
    return state;
}

QState *App::addState(AppState::Values stateEnumVal, std::function< void() > onEnter)
{
    auto state = addState(stateEnumVal);
    connect(state, &QState::entered, this, std::move(onEnter));
    return state;
}

QState *App::addState(AppState::Values stateEnumVal, MemberFunc onEnter)
{
    auto state = addState(stateEnumVal);
    connect(state, &QState::entered, this, onEnter);
    return state;
}

void App::initTimeRange()
{
    QDate currentDate = QDateTime::currentDateTime().date();
    qDebug() << "currentDate = " << currentDate.toString();
    const auto &today = QDateTime(currentDate);
    const auto &tomorrow = QDateTime(currentDate.addDays(1));
    qDebug() << "today:" << today << " tomorrow: " << tomorrow;
    m_start = today;
    m_end = tomorrow;
}

AppState::Values App::state() const
{
    return m_state;
}

void App::setState(AppState::Values state)
{
    qDebug() << __PRETTY_FUNCTION__ << "state=" << state;
    if (m_state != state) {
        m_state = state;
        emit stateChanged(QPrivateSignal());
    }
}

bool App::isLoaded() const
{
    return m_isLoaded;
}

bool App::isConnected() const
{
    return m_isConnected;
}

bool App::isRememberLocationOn() const
{
    return m_isRememberLocationOn;
}

const QString &App::country() const
{
    return m_country;
}

const QString &App::city() const
{
    return m_city;
}

void App::clearEventsModel()
{
    qDebug() << __PRETTY_FUNCTION__;
    m_allEvents.clear();
    m_eventsModel->setEvents({});
    emit todayFoundEventsChanged(QPrivateSignal());
    emit totalFoundEventsChanged(QPrivateSignal());
}

void App::doReload()
{
    qDebug() << "doReload...";
    assignIsLoaded(!m_firstLoad);
    initTimeRange();
    if (m_firstLoad) {
        clearEventsModel();
    }
    if (m_networkAccessManager->networkAccessible() != QNetworkAccessManager::NetworkAccessibility::Accessible) {
        qDebug() << "Network is not accessible!";
        emit loadFailed(QPrivateSignal());
        return;
    }
    QNetworkRequest request;
    QUrl requestUrl(m_eventsRequestUrlBase, QUrl::ParsingMode::TolerantMode);
    QUrlQuery query;
    if (m_city.isEmpty()) {
        query.addQueryItem(QStringLiteral("facets[country][]"), Countries::countryCode(m_country));
    } else {
        auto capitalized = m_city;
        capitalized[0] = capitalized[0].toUpper();
        query.addQueryItem(QStringLiteral("facets[city][]"), capitalized);
    }
    QDateTime yesterday = m_start.addDays(-1);
    query.addQueryItem(QStringLiteral("filter[~and][search_api_aggregation_1][~gt]"),
                       QString::number(yesterday.toSecsSinceEpoch()));
    if (m_firstLoad) {
        query.addQueryItem(QStringLiteral("filter[~or][search_api_aggregation_1][~lt]"),
                           QString::number(m_end.addDays(30).toSecsSinceEpoch()));
    } else {
        query.addQueryItem(QStringLiteral("offset"), QString::number(m_eventsModel->rowCount(QModelIndex())));
        query.addQueryItem(QStringLiteral("limit"), QString::number(100));
    }
    requestUrl.setQuery(query);
    qDebug() << "requestUrl=" << requestUrl.toString();
    request.setUrl(requestUrl);
    request.setRawHeader(QByteArrayLiteral("User-Agent"), QByteArrayLiteral("Radar App 1.0"));

    auto reply = m_networkAccessManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [ this, reply ]() noexcept {
        qDebug() << "============== Events request reply =============";
        qDebug() << "reply.error" << reply->error();
        qDebug() << "reply.isFinished = " << reply->isFinished();
        qDebug() << "reply.url" << reply->url();
        qDebug() << "reply.size:" << reply->size();
        qDebug() << "Content-Type" << reply->header(QNetworkRequest::KnownHeaders::ContentTypeHeader);
        if (reply->isFinished() && reply->error() != QNetworkReply::NetworkError::NoError) {
            qDebug() << "reply failed, removing it...";
            emit this->loadFailed(QPrivateSignal());
            reply->close();
            reply->deleteLater();
            return;
        }
        if (reply->isFinished()) {
            QByteArray buf = reply->readAll();
            QJsonParseError err{};
            QJsonDocument json = QJsonDocument::fromJson(buf, &err);
            if (json.isNull()) {
                qCritical() << "Json parse error:" << err.errorString();
                emit loadFailed(QPrivateSignal());
            }
            m_events = json.object();
            // qDebug() << "result: " << m_events;
            qDebug() << "loadCompleted!";
            m_firstLoad = false;
            emit isFirstLoadChanged(QPrivateSignal());
            emit this->loadCompleted(QPrivateSignal());
            reply->close();
            reply->deleteLater();
        }
    });
    using SignalType = void (QNetworkReply::*)(QNetworkReply::NetworkError);
    connect(reply, static_cast< SignalType >(&QNetworkReply::error),
            this, [this](QNetworkReply::NetworkError error) noexcept {
                qDebug() << "Network error: " << error;
                emit this->loadFailed(QPrivateSignal());
            });
}

void App::doLoadCountries()
{
    qDebug() << "doLoadCountries...";
    updateAllCountries(m_db->getAllCountries());
    if (!m_allCountries.empty()) {
        emit this->countriesAlreadyLoaded(QPrivateSignal());
        return;
    }
    QNetworkRequest request;
    QUrl requestUrl(m_groupsRequestUrl, QUrl::ParsingMode::TolerantMode);
    qDebug() << "requestUrl=" << requestUrl.toString();
    request.setUrl(requestUrl);
    request.setRawHeader(QByteArrayLiteral("User-Agent"), QByteArrayLiteral("Radar App 1.0"));

    auto reply = m_networkAccessManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [ this, reply ]() noexcept {
        qDebug() << "reply.error" << reply->error();
        qDebug() << "reply.isFinished = " << reply->isFinished();
        qDebug() << "reply.url" << reply->url();
        qDebug() << "reply.size:" << reply->size();
        qDebug() << "Content-Type" << reply->header(QNetworkRequest::KnownHeaders::ContentTypeHeader);
        if (reply->isFinished()) {
            QByteArray buf = reply->readAll();
            // qDebug() << buf;
            QJsonParseError err{};
            QJsonDocument json = QJsonDocument::fromJson(buf, &err);
            if (json.isNull()) {
                qCritical() << "Json parse error:" << err.errorString();
                emit loadFailed(QPrivateSignal());
            }
            m_groups = json.object();
            qDebug() << "loadCompleted!";
            emit this->loadCompleted(QPrivateSignal());
            reply->close();
            reply->deleteLater();
        }
    });
}

void App::doLoadCities()
{
    if (m_allCountries.isEmpty()) {
        emit loadFailed(QPrivateSignal());
        return;
    }
    m_citiesByCountryCode.clear();
    m_citiesByCountryCode = m_db->getAllCities();
    if (!m_citiesByCountryCode.empty()) {
        emit citiesAlreadyLoaded(QPrivateSignal());
        updateCurrentLocation();
        return;
    }
    const auto &countryCodes = Countries::allCountries();
    qDebug() << m_allCountries;
    QStringList allCodes;
    allCodes.reserve(countryCodes.size());
    for (const auto &country : qAsConst(m_allCountries)) {
        allCodes.push_back(Countries::countryCode(country));
    }
    m_countriesToLoad = QSet< QString >::fromList(qAsConst(allCodes));

    for (const auto &code : qAsConst(allCodes)) {
        const auto countryCode = code.toUpper();
        QUrl requestUrl(m_cityRequestUrlBase.arg(countryCode));
        qDebug() << "Request URL:" << requestUrl.toString();
        QNetworkRequest request;
        request.setUrl(requestUrl);
        request.setRawHeader(QByteArrayLiteral("User-Agent"), QByteArrayLiteral("Radar App 1.0"));
        auto reply = m_networkAccessManager->get(request);
        connect(reply, &QNetworkReply::finished, this, [ this, reply, countryCode ]() noexcept {
            qDebug() << "reply.error" << reply->error();
            qDebug() << "reply.isFinished = " << reply->isFinished();
            qDebug() << "reply.url" << reply->url();
            qDebug() << "reply.size:" << reply->size();
            qDebug() << "Content-Type" << reply->header(QNetworkRequest::KnownHeaders::ContentTypeHeader);
            if (reply->isFinished()) {
                m_countriesToLoad.remove(countryCode);
                qDebug() << "Remaining countries:" << m_countriesToLoad;
                QByteArray buf = reply->readAll();
                QJsonParseError err{};
                QJsonDocument json = QJsonDocument::fromJson(buf, &err);
                if (json.isNull()) {
                    qCritical() << "Json parse error:" << err.errorString();
                    emit loadFailed(QPrivateSignal());
                }
                const auto &cities = json.object();
                const auto facets = cities.constFind(QLatin1String("facets"));
                if (facets != cities.constEnd()) {
                    const auto &facetsObj = facets->toObject();
                    const auto cityArray = facetsObj.constFind(QLatin1String("city"));
                    if (cityArray != facetsObj.constEnd()) {
                        QStringList citiesForCountry;
                        const auto &jsonArray = cityArray->toArray();
                        citiesForCountry.reserve(jsonArray.size());
                        for (const auto &city : jsonArray) {
                            const auto &cityObj = city.toObject();
                            citiesForCountry.push_back(cityObj.value(QLatin1String("filter")).toString());
                        }
                        qDebug() << "Country: " << countryCode << " Cities:" << citiesForCountry;
                        m_citiesByCountryCode.insert(countryCode, citiesForCountry);
                    }
                }
                reply->close();
                reply->deleteLater();
                if (m_countriesToLoad.empty()) {
                    qDebug() << "cities by country:" << m_citiesByCountryCode;
                    for (auto it = m_citiesByCountryCode.constBegin(), end = m_citiesByCountryCode.constEnd(); it != end;
                         ++it) {
                        m_db->insertCountry(it.key(), Countries::allCountries().key(it.key()), it.value());
                    }
                    emit this->allCitiesLoaded(QPrivateSignal());
                }
            }
        });
    }
}

void App::doFilterCountries()
{
    qDebug() << __PRETTY_FUNCTION__;
    const auto &constEnd = m_groups.constEnd();
    const auto &facets = m_groups.constFind(QLatin1String("facets"));
    QSet< QString > codes;
    if (facets != constEnd) {
        const auto &facetsObj = facets->toObject();
        const auto &countriesIter = facetsObj.constFind(QLatin1String("country"));
        if (countriesIter != facetsObj.constEnd()) {
            const auto &countriesArray = countriesIter->toArray();
            codes.reserve(countriesArray.size());
            for (auto it = countriesArray.constBegin(), cend = countriesArray.constEnd(); it != cend; ++it) {
                const auto countryObj = *it;
                const auto code = countryObj.toObject().value(QLatin1String("filter")).toString();
                codes.insert(code);
            }
        } else {
            qCritical() << "No 'country'";
        }
    } else {
        qCritical() << "No 'facets'";
    }
    const auto countryMap = Countries::allCountries();
    m_allCountries.clear();
    m_allCountries.reserve(codes.size());
    auto it = countryMap.constBegin();
    auto end = countryMap.constEnd();
    while (it != end) {
        if (codes.contains(it.value())) {
            m_allCountries.push_back(it.key());
        }
        ++it;
    }
    qDebug() << "loaded codes:" << codes;
    emit countriesChanged(QPrivateSignal());
    emit countriesFiltered(QPrivateSignal());
}

void App::doCurrentLocationCheck()
{
    if (m_events.isEmpty()) {
        emit noEvents(QPrivateSignal());
        return;
    }
    emit eventsExist(QPrivateSignal());
}

void App::doExtract()
{
    QVector< Event > events;
    QSet< QUuid > locationIDs;
    const auto &constEnd = m_events.constEnd();
    const auto &result = m_events.constFind(QLatin1String("result"));
    if (result != constEnd) {
        auto countIter = m_events.constFind(QLatin1String("count"));
        if (countIter != constEnd) {
            Q_ASSERT(countIter->type() == QJsonValue::Double);
            events.reserve(countIter.value().toInt());
            locationIDs.reserve(events.size());
        }
        const auto &resultObj = result.value().toObject();
        for (const auto &member : qAsConst(resultObj)) {
            const auto &memberObj = member.toObject();
            Event event{};
            event.title = memberObj.value(QLatin1String("title")).toString();
            const auto dateTime = memberObj.value(QLatin1String("date_time")).toArray().first().toObject();
            // qDebug() << "dateTime: " << dateTime.toVariantMap();
            event.timeStart = dateTime.value(QLatin1String("value")).toString().toLongLong();
            event.timeEnd = dateTime.value(QLatin1String("value2")).toString().toLongLong();
            event.date = QDateTime::fromSecsSinceEpoch(event.timeStart).date();
            event.description = memberObj.value(QLatin1String("body")).toObject().value(QLatin1String("value")).toString();
            event.radarUrl = memberObj.value(QLatin1String("url")).toString();
            auto price = memberObj.value(QLatin1String("price"));
            event.price = price.isNull() ? QString() : price.toString();
            const auto &offline = memberObj.value(QLatin1String("offline")).toArray();
            if (!offline.empty()) {
                const auto &offline0 = offline.first().toObject();
                event.rawAddress = offline0.value(QLatin1String("title")).toString();
                const auto &uuid = offline0.value(QLatin1String("id")).toString();
                event.locationID = QUuid::fromString(uuid);
                locationIDs.insert(event.locationID);
            }
            const auto &categoriesIt = memberObj.constFind(QLatin1String("category"));
            if (categoriesIt != memberObj.constEnd()) {
                const auto &categories = categoriesIt->toArray();
                for (const auto &item : categories) {
                    event.category += item.toObject().value(QStringLiteral("name")).toString();
                    event.category.append(QLatin1String(" | "));
                }
                event.category.chop(3);
            }
            events.push_back(event);
        }
    }
    locationIDs.squeeze();
    qDebug() << "Locations count:" << locationIDs.size();
    //    m_allEvents.append(events);
    m_allEvents = std::move(events);
    m_locationProvider->setNetworkAccessManager(m_networkAccessManager);

    auto capitalized = m_city;
    capitalized[0] = capitalized[0].toUpper();

    m_locationProvider->setLocationsToLoad(std::move(locationIDs), Countries::countryCode(m_country), capitalized);
    emit eventListReady(QPrivateSignal());
}

void App::doFiltering()
{
    QVector< Event > filtered;

    const auto startSeconds = m_start.toSecsSinceEpoch();
    const auto endSeconds = m_end.toSecsSinceEpoch();

    qDebug() << "start: " << m_start.toString() << " startSeconds=" << startSeconds;
    qDebug() << "end: " << m_end.toString() << " endSeconds=" << endSeconds;

    std::copy_if(m_allEvents.cbegin(), m_allEvents.cend(), std::back_inserter(filtered),
                 [ startSeconds, endSeconds ](const Event &event) noexcept {
                     Q_UNUSED(event)
                     Q_UNUSED(startSeconds)
                     Q_UNUSED(endSeconds)
                     // qDebug() << "event start " << event.timeStart << " end " << event.timeEnd;
                     return true;// event.timeStart >= startSeconds && event.timeEnd <= endSeconds;
                 });

    std::sort(filtered.begin(), filtered.end(), [](const Event &left, const Event &right) noexcept {
        return left.timeStart < right.timeStart;
    });
    m_eventsModel->appendEvents(std::move(filtered));
    emit eventsModelChanged(QPrivateSignal());
    emit eventListFiltered(QPrivateSignal());
    emit totalFoundEventsChanged(QPrivateSignal());
    emit todayFoundEventsChanged(QPrivateSignal());
}

void App::getPermissions()
{
#ifdef Q_OS_ANDROID
    static QString internet{QStringLiteral("android.permission.INTERNET")};
    static QString networkState{QStringLiteral("android.permission.ACCESS_NETWORK_STATE")};
    static QStringList permissions{internet, networkState};
    const auto accessInternetResult = QtAndroid::checkPermission(internet);
    if (accessInternetResult == QtAndroid::PermissionResult::Granted) {
        qDebug() << "Has INTERNET permission!";
        const auto accessNetworkStateResult = QtAndroid::checkPermission(networkState);
        if (accessNetworkStateResult == QtAndroid::PermissionResult::Granted) {
            qDebug() << "Has ACCESS_NETWORK_STATE permission!";
            // emit gotPermisssions(QPrivateSignal());
        } else {
            qDebug() << "No ACCESS_NETWORK_STATE permission!";
            emit failedToGetPermissions(QPrivateSignal());
        }
    } else {
        qDebug() << "No INTERNET permission!";
        emit failedToGetPermissions(QPrivateSignal());
    }
    QtAndroid::requestPermissions(permissions, [this](const QtAndroid::PermissionResultMap &results) noexcept {
        if (results.value(internet, QtAndroid::PermissionResult::Denied) == QtAndroid::PermissionResult::Granted
            && results.value(networkState, QtAndroid::PermissionResult::Denied) == QtAndroid::PermissionResult::Granted) {
            qDebug() << "Both permissions granted!";
            emit this->gotPermissions(QPrivateSignal());
            return;
        }
        qDebug() << "Some permissions not granted!";
        for (auto it = results.cbegin(), cend = results.cend(); it != cend; ++it) {
            qDebug() << it.key() << " granted=" << (it.value() == QtAndroid::PermissionResult::Granted);
        }
        emit this->failedToGetPermissions(QPrivateSignal());
    });
#else
    emit gotPermissions(QPrivateSignal());
#endif
}

void App::reload()
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!m_fsm.isRunning()) {
        connect(&m_fsm, &QStateMachine::runningChanged, this, [this](bool running) {
            if (running) {
                qDebug() << "requesting reload!";
                emit this->reloadRequested(QPrivateSignal());
                disconnect(&m_fsm, &QStateMachine::runningChanged, this, nullptr);
            }
        });
    } else {
        qDebug() << "requesting reload!";
        emit reloadRequested(QPrivateSignal());
    }
}

void App::reloadEvents()
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!m_fsm.isRunning()) {
        connect(&m_fsm, &QStateMachine::runningChanged, this, [this](bool running) {
            if (running) {
                qDebug() << "requesting reloadEvents!";
                emit this->reloadEventsRequested(QPrivateSignal());
                disconnect(&m_fsm, &QStateMachine::runningChanged, this, nullptr);
            }
        });
    } else {
        qDebug() << "requesting reloadEvents!";
        emit reloadEventsRequested(QPrivateSignal());
    }
}

void App::selectEvent(int index)
{
    m_currentEvent = m_eventsModel->getEventByIndex(index);
    bool alreadyLoaded;
    std::tie(m_currentLocation, alreadyLoaded) = m_locationProvider->getLoadedLocation(m_currentEvent.locationID);
    emit currentEventChanged(QPrivateSignal());
    emit currentLocationChanged(QPrivateSignal());
    if (!alreadyLoaded) {
        m_locationProvider->requestLocation(m_currentEvent.locationID);
    }
}

void App::openLink(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}

void App::rememberSelectedLocation()
{
    QSettings settings;
    settings.setValue(settingsCountryKey, m_country);
    settings.setValue(settingsCityKey, m_city);
}

void App::setCity(const QString &city)
{
    qDebug() << "setCity:" << city;
    qDebug() << "m_city:" << m_city;
    if (!isLoaded() || m_city != city) {
        assignIsLoaded(false);
        m_eventsModel->setEvents({});
        m_city = city;
        m_firstLoad = true;
        emit isFirstLoadChanged(QPrivateSignal());
        emit cityChanged(QPrivateSignal());
        reloadEvents();
    }
}

void App::showLocation()
{
    qDebug() << "Location Address:" << locationAddress() << "Location Name:" << locationName();
#ifdef Q_OS_ANDROID
    QString geoUrl = m_currentLocation.coordinate.isValid()
                         ? QStringLiteral("geo:%1,%2?q=%1,%2").arg(latitude()).arg(longitude())
                         : QStringLiteral("geo:0,0");
    if (!m_currentLocation.coordinate.isValid()) {
        QUrl url = QUrl(geoUrl);
        QUrlQuery query;
        const auto locationQuery = locationAddress()
                                       .replace(QChar(','), QChar(' '))
                                       .split(QChar(' '), QString::SplitBehavior::SkipEmptyParts)
                                       .join(QChar('+'));
        query.addQueryItem(QStringLiteral("q"), locationQuery);
        url.setQuery(query);
        qDebug() << "url=" << url;
        geoUrl = url.toString();
    }
    qDebug() << "geoURL=" << geoUrl;
    QAndroidJniObject geoJavaString = QAndroidJniObject::fromString(geoUrl);
    QAndroidJniObject uri = QAndroidJniObject::callStaticObjectMethod(
        "android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", geoJavaString.object< jobject >());

    QAndroidIntent intent(QStringLiteral("android.intent.action.VIEW"));
    intent.handle().callObjectMethod("setData", "(Landroid/net/Uri;)Landroid/content/Intent;", uri.object< jobject >());
    QtAndroid::startActivity(intent.handle(), 0, nullptr);
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        qCritical() << "Exception:";
        env->ExceptionDescribe();
        env->ExceptionClear();
        emit failedToOpenMapApp(QPrivateSignal());
    }
#endif
}

void App::addToCalendar()
{
    qDebug() << __PRETTY_FUNCTION__;
#ifdef Q_OS_ANDROID
    QAndroidJniEnvironment env;
    auto getCalendarContractEventsColumnsStaticField = [](const char *field, const char *sig) {
        auto fieldId
            = QAndroidJniObject::getStaticObjectField("android/provider/CalendarContract$EventsColumns", field, sig);
        qDebug() << "field name: " << field << ", signature: " << sig << " fieldID: " << fieldId.object();
        return fieldId;
    };

    QAndroidIntent intent(QStringLiteral("android.intent.action.INSERT"));
    qDebug() << "setting CONTENT_URI...";
    auto contentUri = QAndroidJniObject::getStaticObjectField("android/provider/CalendarContract$Events", "CONTENT_URI",
                                                              "Landroid/net/Uri;");
    intent.handle().callObjectMethod("setData", "(Landroid/net/Uri;)Landroid/content/Intent;",
                                     contentUri.object< jobject >());
    qDebug() << "setting TITLE...";
    auto eventTitleJniObject = getCalendarContractEventsColumnsStaticField("TITLE", "Ljava/lang/String;");
    auto titleJniObject
        = QAndroidJniObject::fromString(QStringLiteral("%1 at %2").arg(m_currentEvent.title, locationName()));
    qDebug() << "Setting event title to " << m_currentEvent.title;
    intent.handle().callObjectMethod("putExtra", "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;",
                                     eventTitleJniObject.object< jobject >(), titleJniObject.object< jobject >());
    auto beginTimeExtra = QAndroidJniObject::getStaticObjectField("android/provider/CalendarContract",
                                                                  "EXTRA_EVENT_BEGIN_TIME", "Ljava/lang/String;");
    intent.handle().callObjectMethod("putExtra", "(Ljava/lang/String;J)Landroid/content/Intent;",
                                     beginTimeExtra.object< jobject >(), m_currentEvent.timeStart * 1000ll);
    auto endTimeExtra = QAndroidJniObject::getStaticObjectField("android/provider/CalendarContract", "EXTRA_EVENT_END_TIME",
                                                                "Ljava/lang/String;");
    intent.handle().callObjectMethod("putExtra", "(Ljava/lang/String;J)Landroid/content/Intent;",
                                     endTimeExtra.object< jobject >(), m_currentEvent.timeEnd * 1000ll);
    qDebug() << "setting DESCRIPTION...";
    auto descriptionExtra = getCalendarContractEventsColumnsStaticField("DESCRIPTION", "Ljava/lang/String;");
    intent.handle().callObjectMethod("putExtra", "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;",
                                     descriptionExtra.object< jobject >(),
                                     QAndroidJniObject::fromString(m_currentEvent.description).object< jobject >());

    qDebug() << "setting EVENT_LOCATION...";
    auto locationExtra = getCalendarContractEventsColumnsStaticField("EVENT_LOCATION", "Ljava/lang/String;");
    intent.handle().callObjectMethod("putExtra", "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;",
                                     locationExtra.object< jobject >(),
                                     QAndroidJniObject::fromString(locationAddress()).object< jobject >());

    qDebug() << "Starting activity...";
    QtAndroid::startActivity(intent.handle(), 0);
    if (env->ExceptionCheck()) {
        qCritical() << "Exception:";
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
#endif
}

void App::stopUpdatePosition()
{
    m_eventsModel->stopUpdatePosition();
}

void App::startUpdatePosition()
{
    m_eventsModel->startUpdatePosition();
    // m_eventsModel->forceUpdatePosition();
}

void App::doSharing(const QString &title, const QString &body)
{
#ifndef Q_OS_ANDROID
    {
        QUrl url;
        url.setScheme(QStringLiteral("mailto"));
        QUrlQuery query;
        query.setQueryItems({
            {QStringLiteral("subject"), title},
            {QStringLiteral("body"), body},
        });
        url.setQuery(query);
        QDesktopServices::openUrl(url);
    }
#else
    auto action
        = QAndroidJniObject::getStaticObjectField("android/content/Intent", "ACTION_SEND", "Ljava/lang/String;").toString();
    qDebug() << "action=" << action;
    auto extraSubject
        = QAndroidJniObject::getStaticObjectField("android/content/Intent", "EXTRA_SUBJECT", "Ljava/lang/String;");
    auto extraText = QAndroidJniObject::getStaticObjectField("android/content/Intent", "EXTRA_TEXT", "Ljava/lang/String;");
    QAndroidIntent intent(action);
    intent.handle().callObjectMethod("putExtra", "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;",
                                     extraSubject.object< jstring >(),
                                     QAndroidJniObject::fromString(title).object< jstring >());
    intent.handle().callObjectMethod("putExtra", "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;",
                                     extraText.object< jstring >(), QAndroidJniObject::fromString(body).object< jstring >());
    intent.handle().callObjectMethod("setType", "(Ljava/lang/String;)Landroid/content/Intent;",
                                     QAndroidJniObject::fromString(QStringLiteral("text/plain")).object< jstring >());
    auto chooser = QAndroidJniObject::callStaticObjectMethod(
        "android/content/Intent", "createChooser",
        "(Landroid/content/Intent;Ljava/lang/CharSequence;)Landroid/content/Intent;", intent.handle().object(),
        QAndroidJniObject::fromString(QStringLiteral("Share URL")).object< jstring >());
    QtAndroid::startActivity(chooser, 0, nullptr);

    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        qCritical() << "Exception:";
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
#endif
}

void App::share()
{
    qDebug() << "Sharing event " << m_currentEvent.title << " with URL " << eventUrl();
    const auto &linkTitle = title();
    const auto &body = sharableBody();

    doSharing(linkTitle, body);
}

void App::shareApp()
{
    doSharing(tr("Radar App"), tr("Link to Android APK: %1").arg(m_downloadLink));
}

void App::toggleRememberLocation()
{
    m_isRememberLocationOn = !m_isRememberLocationOn;
    if (m_isRememberLocationOn && m_isLoaded) {
        rememberSelectedLocation();
    }
    emit rememberLocationChanged(QPrivateSignal());
}

void App::cancelOperation()
{
    emit userCancelled(QPrivateSignal());
}

void App::resetNetworkConnection()
{
    m_networkAccessManager->clearConnectionCache();
}

void App::refreshCountries()
{
    m_isLoaded = false;
    m_countriesToLoad.clear();
    m_citiesByCountryCode.clear();

    m_db->clearCountries();
    m_db->clearCities();
    reload();
}

int App::getFirstTodaysItemIndex() const
{
    return m_eventsModel->todaysFirstEventIndex();
}

QAbstractListModel *App::eventsModel() const
{
    return m_eventsModel;
}

bool App::noEventsFound() const
{
    return m_eventsModel->rowCount(QModelIndex()) == 0;
}

bool App::isFirstLoad() const
{
    return m_firstLoad;
}

const QString &App::title() const
{
    return m_currentEvent.title;
}

const QString &App::description() const
{
    return m_currentEvent.description;
}

QString App::dateTime() const
{
    const auto dateTime = QDateTime::fromSecsSinceEpoch(m_currentEvent.timeStart);
    return QStringLiteral("%1, %2").arg(dateTime.date().toString(Qt::SystemLocaleLongDate),
                                        dateTime.time().toString(QStringLiteral("HH:mm")));
}

QString App::duration() const
{
    if (m_currentEvent.timeStart >= m_currentEvent.timeEnd) {
        return QString();
    }
    auto start = m_currentEvent.timeStart;
    auto end = m_currentEvent.timeEnd;
    auto secsTo = end - start;
    auto days = secsTo / 86400;
    auto hours = (secsTo / 3600) % 24;
    auto minutes = static_cast< qulonglong >(secsTo / 60) % 60;
    if (days) {
        return tr("%1 days, %2 hours %3 minutes").arg(days).arg(hours).arg(minutes);
    }
    return tr("%1:%2").arg(hours).arg(minutes, 2, 10, QLatin1Char('0'));
}

const QString &App::category() const
{
    return m_currentEvent.category;
}

const QString &App::price() const
{
    return m_currentEvent.price;
}

QString App::locationName() const
{
    return m_currentLocation.name;
}

QString App::locationAddress() const
{
    if (m_currentLocation.uuid.isNull()) {
        return m_currentEvent.rawAddress;
    }

    if (m_currentLocation.postalCode != 0) {
        return QStringLiteral("%1, %2, %3")
            .arg(m_currentLocation.postalCode)
            .arg(m_currentLocation.locality, m_currentLocation.thoroughfare);
    }
    if (m_currentLocation.thoroughfare.length() > 0) {
        return QStringLiteral("%1, %2").arg(m_currentLocation.locality, m_currentLocation.thoroughfare);
    }
    return tr("%1, %2").arg(QObject::tr(qPrintable(Countries::countryByCode(m_currentLocation.country))),
                            m_currentLocation.locality);
}

QString App::eventCity() const
{
    return m_currentLocation.locality;
}

QString App::eventCountry() const
{
    return m_currentLocation.country;
}

QString App::directions() const
{
    return m_currentLocation.directions;
}

QString App::eventUrl() const
{
    return m_currentEvent.radarUrl;
}

qreal App::longitude() const
{
    return m_currentLocation.coordinate.isValid() ? m_currentLocation.coordinate.longitude() : ::qQNaN();
}

qreal App::latitude() const
{
    return m_currentLocation.coordinate.isValid() ? m_currentLocation.coordinate.latitude() : ::qQNaN();
}

QStringList App::countries() const
{
    return m_allCountries;
}

void App::forceSetCountry(const QString &country)
{
    m_country = country;
    emit countryChanged(QPrivateSignal());
    const auto &citiesForCountry = cities();

    setCity(citiesForCountry.empty() ? QString()
                                     : (citiesForCountry.contains(m_city) ? m_city : citiesForCountry.constFirst()));
    emit citiesChanged(QPrivateSignal());
    emit cityChanged(QPrivateSignal());
    reloadEvents();
}

void App::updateAllCountries(const QStringList &countries)
{
    if (m_allCountries != countries) {
        m_allCountries = countries;
        emit countriesChanged(QPrivateSignal());
    }
}

void App::assignIsLoaded(bool loaded)
{
    if (m_isLoaded != loaded) {
        m_isLoaded = loaded;
        emit isLoadedChanged(QPrivateSignal());
    }
}

QString App::sharableBody() const
{
    QTextDocument message;
    message.setHtml(description());
    return tr("%1\nDate: %2\n%3\n\nLink: %4").arg(title(), dateTime(), message.toPlainText(), eventUrl());
}

void App::setCountry(const QString &country)
{
    if (m_country != country) {
        qDebug() << "Changing country to " << country;
        forceSetCountry(country);
    }
}

QStringList App::cities() const
{
    auto cities = m_citiesByCountryCode.value(Countries::countryCode(m_country), {});
    cities.sort();
    return cities;
}

int App::totalFoundEvents() const
{
    return m_eventsModel->rowCount(QModelIndex());
}

int App::todayFoundEvents() const
{
    return m_eventsModel->todayEventsCount();
}

void App::updateCurrentLocation()
{
    if (!m_allCountries.contains(m_country)) {
        if (m_allCountries.empty()) {
            return;
        }
        m_country = m_allCountries.at(0);
    }
    forceSetCountry(m_country);
}
