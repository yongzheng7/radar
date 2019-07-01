#include "locationprovider.h"
#include "database.h"

#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

LocationProvider::LocationProvider(QObject *parent)
    : QObject(parent)
{
}

LocationProvider::~LocationProvider()
{
}

Location LocationProvider::extractLocationFromJSON(const QJsonDocument &json)
{
    QJsonObject obj = json.object();
    qDebug() << "obj: " << obj.toVariantMap();
    Location location{};
    const auto &address = obj.value(QLatin1Literal("address")).toObject();
    location.country = address.value(QLatin1Literal("country")).toString();
    location.locality = address.value(QLatin1Literal("locality")).toString();
    location.firstName = address.value(QLatin1Literal("first_name")).toString();
    location.lastName = address.value(QLatin1Literal("last_name")).toString();
    location.postalCode = address.value(QLatin1Literal("postal_code")).toString().toInt();
    location.thoroughfare = address.value(QLatin1Literal("thoroughfare")).toString();
    location.name = address.value(QLatin1Literal("name_line")).toString();

    location.directions = obj.value(QLatin1Literal("directions")).toString();

    const auto &map = obj.value(QLatin1Literal("map")).toObject();
    bool okLon, okLat;
    qreal latitude = map.value(QLatin1Literal("lat")).toString().toDouble(&okLat);
    qreal longitude = map.value(QLatin1Literal("lon")).toString().toDouble(&okLon);
    if (okLat && okLon) {
        location.coordinate.setLatitude(latitude);
        location.coordinate.setLongitude(longitude);
    }
    qDebug() << "name: " << location.name << " address:" << location.thoroughfare;
    QUuid uuid = QUuid::fromString(obj.value(QLatin1Literal("uuid")).toString());
    location.uuid = uuid;
    return location;
}

void LocationProvider::processFinishedReply(QNetworkReply *reply)
{
    qDebug() << "reply.error" << reply->error();
    qDebug() << "reply.isFinished = " << reply->isFinished();
    qDebug() << "reply.url" << reply->url();
    qDebug() << "reply.size:" << reply->size();
    qDebug() << "Content-Type" << reply->header(QNetworkRequest::KnownHeaders::ContentTypeHeader);
    if (!reply->isFinished()) {
        return;
    }
    if (reply->error() != QNetworkReply::NoError) {
        qCritical() << "Error processing request " << reply->request().url() << " Error:" << reply->error();
    }
    if (!reply->isFinished()) {
        return;
    }
    QByteArray buf = reply->readAll();
    QJsonParseError err;
    QJsonDocument json = QJsonDocument::fromJson(buf, &err);
    if (json.isNull()) {
        qCritical() << "Json parse error:" << err.errorString();
        return;
    }
    auto location = extractLocationFromJSON(json);

    m_loadedLocations.insert(location.uuid, location);
    m_db->insertLocation(location);
    m_locationsToLoad.remove(location.uuid);

    emit locationAvailable(location.uuid, location);
    reply->close();
    reply->deleteLater();
}

void LocationProvider::setNetworkAccessManager(QNetworkAccessManager *networkAccessManager)
{
    m_networkAccessManager = networkAccessManager;
}

void LocationProvider::requestLocation(const QUuid &uuid)
{
    const auto foundIt = m_loadedLocations.constFind(uuid);
    if (foundIt != m_loadedLocations.constEnd()) {
        qDebug() << "Already loaded!";
        emit locationAvailable(uuid, foundIt.value());
        return;
    }
    Location location;
    bool found;
    std::tie(found, location) = m_db->findLocation(uuid);
    if (found) {
        qDebug() << "found in DB!";
        m_loadedLocations.insert(uuid, location);
        emit locationAvailable(uuid, location);
        return;
    }
    qDebug() << "Download needed!";
    m_locationsToLoad.insert(uuid);
    doLoad(uuid);
}

std::pair< Location, bool > LocationProvider::getLoadedLocation(const QUuid &uuid) const
{
    const auto found = m_loadedLocations.constFind(uuid);
    if (found == m_loadedLocations.constEnd()) {
        return std::make_pair< Location, bool >({}, false);
    }
    return {*found, true};
}

void LocationProvider::setLocationsToLoad(QSet< QUuid > &&locations)
{
    m_locationsToLoad = std::move(locations);
    auto it = m_locationsToLoad.begin();
    const auto loadedEnd = m_loadedLocations.cend();
    while (it != m_locationsToLoad.end()) {
        const auto fountIt = m_loadedLocations.constFind(*it);
        if (fountIt != loadedEnd) {
            emit locationAvailable(fountIt.key(), fountIt.value());
            it = m_locationsToLoad.erase(it);
        } else {
            ++it;
        }
    }
    m_locationsToLoad.subtract(m_db->getAllUUIDs());
    loadAllLocations();
}

QNetworkReply *LocationProvider::requestLocationByUUID(const QUuid &id)
{
    QUrl requestUrl(
        QStringLiteral("https://radar.squat.net/api/1.2/location/%1.json").arg(id.toString(QUuid::WithoutBraces)));
    QNetworkRequest request(requestUrl);
    request.setRawHeader(QByteArrayLiteral("User-Agent"), QByteArrayLiteral("Radar App 1.0"));
    qDebug() << "[Network] Requesting location " << id;
    return m_networkAccessManager->get(request);
}

void LocationProvider::doLoad(const QUuid &id)
{
    // load over network
    auto reply = requestLocationByUUID(id);
    connect(reply, &QNetworkReply::finished, this, [ this, reply ]() noexcept { this->processFinishedReply(reply); });
}

void LocationProvider::loadAllLocations()
{
    qDebug() << "Locations remained to load from network=" << m_locationsToLoad.size();
    for (const auto &uuid : qAsConst(m_locationsToLoad)) {
        doLoad(uuid);
    }
}

void LocationProvider::setDB(DB *db)
{
    m_db = db;
}
