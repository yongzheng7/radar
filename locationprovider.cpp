#include "locationprovider.h"
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

void LocationProvider::onResponse(QNetworkReply *reply)
{
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
            return;
        }
        QJsonObject obj = json.object();
        qDebug() << "obj: " << obj.toVariantMap();
        Location location {};
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
        location.latitude = map.value(QLatin1Literal("lat")).toString();
        location.longitude = map.value(QLatin1Literal("lon")).toString();
        qDebug() << "name: " << location.name << " address:" << location.thoroughfare;
        QUuid uuid = QUuid::fromString(obj.value(QLatin1Literal("uuid")).toString());
        location.uuid = uuid;

        m_loadedLocations.insert(uuid, location);
        m_locationsToLoad.remove(uuid);

        emit locationAvailable(uuid, location);
        reply->deleteLater();
    }
}

void LocationProvider::setNetworkAccessManager(QNetworkAccessManager *networkAccessManager)
{
    if (m_networkAccessManager) {
        disconnect(m_networkAccessManager, nullptr, this, nullptr);
    }
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
    qDebug() << "Download needed!";
    m_locationsToLoad.insert(uuid);
    doLoad(uuid);
}

std::pair<Location, bool> LocationProvider::getLoadedLocation(const QUuid &uuid) const
{
    const auto found = m_loadedLocations.constFind(uuid);
    if (found == m_loadedLocations.constEnd()) {
        return std::make_pair<Location, bool>({}, false);
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
}

void LocationProvider::doLoad(const QUuid &id)
{
    // load over network or from db
    connect(m_networkAccessManager, &QNetworkAccessManager::finished, this, &LocationProvider::onResponse, Qt::UniqueConnection);
    QUrl requestUrl(QStringLiteral("https://radar.squat.net/api/1.2/location/%1.json").arg(id.toString(QUuid::WithoutBraces)));
    QNetworkRequest request(requestUrl);
    request.setRawHeader(QByteArrayLiteral("User-Agent"), QByteArrayLiteral("Radar App 1.0"));
    m_networkAccessManager->get(request);
}
