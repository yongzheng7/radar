#include "app.h"

#include "eventsmodel.h"
#include "locationprovider.h"

#include <algorithm>

#include <QtDebug>

#include <QJsonObject>
#include <QUrl>
#include <QtNetwork>

#include <QtGlobal>

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QtAndroidExtras/QAndroidIntent>
#endif

namespace
{
    const QString settingsCityKey{QStringLiteral("filter/city")};
}
App::App(QObject *parent)
    : QObject(parent)
    , m_networkAccessManager(new QNetworkAccessManager(this))
    , m_locationProvider(new LocationProvider(this))
    , m_db(new DB(this))
    , m_eventsModel(new EventsModel(this))
{
    qRegisterMetaType< Event >();
    qRegisterMetaType< Location >();
    m_locationProvider->setNetworkAccessManager(m_networkAccessManager);
    QSqlError err = m_db->initDB();
    if (err.type() != QSqlError::NoError) {
        qCritical() << err.text();
        assert(!"Failed to init DB!");
    }
    m_locationProvider->setDB(m_db);

    initTimeRange();
    QSettings settings;

    m_city = settings.value(settingsCityKey, QStringLiteral("Berlin")).toString();

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
}

App::~App()
{
}

void App::setupFSM()
{
    auto idle = addState(AppState::Values::Idle);
    auto loading = addState(AppState::Values::Loading, &App::doReload);
    auto extraction = addState(AppState::Values::Extraction, &App::doExtract);
    auto filtering = addState(AppState::Values::Filtering, &App::doFiltering);
#ifdef Q_OS_ANDROID
    auto requestingPermissions = addState(AppState::Values::Permission, &App::getPermissions);
    requestingPermissions->addTransition(this, &App::gotPermissions, loading);
    requestingPermissions->addTransition(this, &App::failedToGetPermissions, idle);
#endif
    m_fsm.setInitialState(idle);

    connect(&m_fsm, &QStateMachine::runningChanged, this, [](bool running) { qDebug() << "runningChanged to " << running; });

#ifndef Q_OS_ANDROID
    idle->addTransition(this, &App::reloadRequested, loading);
#else
    idle->addTransition(this, &App::reloadRequested, requestingPermissions);
#endif
    loading->addTransition(this, &App::loadCompleted, extraction);
    extraction->addTransition(this, &App::eventListReady, filtering);
    filtering->addTransition(this, &App::eventListFiltered, idle);
}

QState *App::addState(AppState::Values stateEnumVal)
{
    auto state = new QState(&m_fsm);
    state->assignProperty(this, "state", QVariant::fromValue< AppState::Values >(stateEnumVal));
    return state;
}

QState *App::addState(AppState::Values stateEnumVal, std::function< void() > onEnter)
{
    auto state = addState(stateEnumVal);
    connect(state, &QState::entered, this, onEnter);
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
    qDebug() << __PRETTY_FUNCTION__;
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

const QString &App::country() const
{
    return m_country;
}

const QString &App::city() const
{
    return m_city;
}

void App::doReload()
{
    qDebug() << "doReload...";
    QNetworkRequest request;
    QUrl requestUrl(m_eventsRequestUrlBase, QUrl::ParsingMode::TolerantMode);
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("facets[city][]"), m_city);
    requestUrl.setQuery(query);
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
            QJsonParseError err;
            QJsonDocument json = QJsonDocument::fromJson(buf, &err);
            if (json.isNull()) {
                qCritical() << "Json parse error:" << err.errorString();
                emit loadFailed(QPrivateSignal());
            }
            m_events = json.object();
            // qDebug() << "result: " << m_events;
            qDebug() << "loadCompleted!";
            emit this->loadCompleted(QPrivateSignal());
            reply->close();
            reply->deleteLater();
        }
    });
}

void App::doExtract()
{
    disconnect(m_networkAccessManager, nullptr, this, nullptr);
    QVector< Event > events;
    QSet< QUuid > locationIDs;
    const auto &constEnd = m_events.constEnd();
    const auto &result = m_events.constFind(QLatin1Literal("result"));
    if (result != constEnd) {
        auto countIter = m_events.constFind(QLatin1Literal("count"));
        if (countIter != constEnd) {
            Q_ASSERT(countIter->type() == QJsonValue::Double);
            events.reserve(countIter.value().toInt());
            locationIDs.reserve(events.size());
        }
        const auto &resultObj = result.value().toObject();
        for (const auto &member : qAsConst(resultObj)) {
            const auto &memberObj = member.toObject();
            Event event{};
            event.title = memberObj.value(QLatin1Literal("title")).toString();
            const auto dateTime = memberObj.value(QLatin1Literal("date_time")).toArray().first().toObject();
            // qDebug() << "dateTime: " << dateTime.toVariantMap();
            event.timeStart = dateTime.value(QLatin1Literal("value")).toString().toLongLong();
            event.timeEnd = dateTime.value(QLatin1Literal("value2")).toString().toLongLong();
            event.date = QDateTime::fromSecsSinceEpoch(event.timeStart).date();
            event.description = memberObj.value(QLatin1Literal("body")).toObject().value(QLatin1Literal("value")).toString();
            const auto &offline = memberObj.value(QLatin1Literal("offline")).toArray();
            if (!offline.empty()) {
                const auto &offline0 = offline.first().toObject();
                event.rawAddress = offline0.value(QLatin1Literal("title")).toString();
                const auto &uuid = offline0.value(QLatin1Literal("id")).toString();
                event.locationID = QUuid::fromString(uuid);
                locationIDs.insert(event.locationID);
            }
            events.push_back(event);
        }
    }
    locationIDs.squeeze();
    qDebug() << "Locations count:" << locationIDs.size();
    m_allEvents = std::move(events);
    m_locationProvider->setLocationsToLoad(std::move(locationIDs));
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
    m_eventsModel->setEvents(std::move(filtered));
    m_locationProvider->setNetworkAccessManager(m_networkAccessManager);
    emit eventsModelChanged(QPrivateSignal());
    emit eventListFiltered(QPrivateSignal());
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
    if (!m_fsm.isRunning()) {
        connect(&m_fsm, &QStateMachine::runningChanged, this, [this](bool running) {
            if (running) {
                emit this->reloadRequested(QPrivateSignal());
                disconnect(&m_fsm, &QStateMachine::runningChanged, this, nullptr);
            }
        });
    } else {
        emit reloadRequested(QPrivateSignal());
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

void App::setCity(const QString &city)
{
    qDebug() << "setCity:" << city;
    if (m_city != city) {
        m_city = city;
        QSettings settings;
        settings.setValue(settingsCityKey, city);
        emit cityChanged(QPrivateSignal());
        reload();
    }
}

void App::showLocation(const QString &location)
{
    qDebug() << "Open location: " << location;
#ifdef Q_OS_ANDROID
    QString geoUrl = QStringLiteral("geo:0,0?q=%1")
                         .arg(location.split(QChar(' '), QString::SplitBehavior::SkipEmptyParts).join(QChar('+')));
    QAndroidJniObject geoJavaString = QAndroidJniObject::fromString(geoUrl);
    QAndroidJniObject uri = QAndroidJniObject::callStaticObjectMethod(
        "android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", geoJavaString.object< jobject >());

    QAndroidJniObject activity = QtAndroid::androidActivity();
    QAndroidIntent intent(QStringLiteral("android.intent.action.VIEW"));
    intent.handle().callObjectMethod("setData", "(Landroid/net/Uri;)Landroid/content/Intent;", uri.object< jobject >());
    QtAndroid::startActivity(intent.handle(), 0, nullptr);
#endif
}

EventsModel *App::eventsModel() const
{
    return m_eventsModel;
}

const QString &App::title() const
{
    return m_currentEvent.title;
}

const QString &App::description() const
{
    return m_currentEvent.description;
}

const QString App::dateTime() const
{
    const auto dateTime = QDateTime::fromSecsSinceEpoch(m_currentEvent.timeStart);
    return QStringLiteral("%1, %2")
        .arg(dateTime.date().toString(Qt::SystemLocaleLongDate))
        .arg(dateTime.time().toString(QStringLiteral("HH:mm")));
}

const QString &App::category() const
{
    return m_currentEvent.category;
}

const QString &App::price() const
{
    static QString unknown = QStringLiteral("Unknown");// m_currentEvent.price;
    return unknown;
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
    } else {
        return QStringLiteral("%1, %2").arg(m_currentLocation.locality, m_currentLocation.thoroughfare);
    }
}

const QString &App::eventCity() const
{
    // FIXME
    static QString empty;
    return empty;
}

const QString &App::eventCountry() const
{
    // FIXME
    static QString empty;
    return empty;
}

QString App::directions() const
{
    return m_currentLocation.directions;
}
