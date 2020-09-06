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

LocationProvider::~LocationProvider() = default;

Location LocationProvider::extractLocationFromJSONDocument(const QJsonDocument &json)
{
    QJsonObject obj = json.object();
    //qDebug() << "obj: " << obj.toVariantMap();
    return extractLocationFromJSONObject(obj);
}

Location LocationProvider::extractLocationFromJSONObject(const QJsonObject &obj)
{
    Location location{};

    {
        const auto &address = obj.value(QLatin1Literal("address")).toObject();
        location.country = address.value(QLatin1Literal("country")).toString();
        location.locality = address.value(QLatin1Literal("locality")).toString();
        location.firstName = address.value(QLatin1Literal("first_name")).toString();
        location.lastName = address.value(QLatin1Literal("last_name")).toString();
        location.postalCode = address.value(QLatin1Literal("postal_code")).toString().toInt();
        location.thoroughfare = address.value(QLatin1Literal("thoroughfare")).toString();
        location.name = address.value(QLatin1Literal("name_line")).toString();
    }

    location.directions = obj.value(QLatin1Literal("directions")).toString();

    {
        const auto &map = obj.value(QLatin1Literal("map")).toObject();
        bool okLon = false;
        bool okLat = false;
        qreal latitude = map.value(QLatin1Literal("lat")).toString().toDouble(&okLat);
        qreal longitude = map.value(QLatin1Literal("lon")).toString().toDouble(&okLon);
        if (okLat && okLon) {
            location.coordinate.setLatitude(latitude);
            location.coordinate.setLongitude(longitude);
        }
    }
    qDebug() << "name: " << location.name << " address:" << location.thoroughfare;
    QUuid uuid = QUuid::fromString(obj.value(QLatin1Literal("uuid")).toString());
    location.uuid = uuid;
    return location;
}

void LocationProvider::processFinishedReply(QNetworkReply *reply)
{
    if (reply == nullptr) {
        return;
    }
    // qDebug() << __PRETTY_FUNCTION__;
    // qDebug() << "reply.error" << reply->error();
    // qDebug() << "reply.isFinished = " << reply->isFinished();
    // qDebug() << "reply.size:" << reply->size();
    // qDebug() << "Content-Type" << reply->header(QNetworkRequest::KnownHeaders::ContentTypeHeader);
    if (!reply->url().toString().startsWith(m_locationUrlBase)) {
        return;
    }
    qDebug() << "reply.url" << reply->url();

    if (reply->error() != QNetworkReply::NoError) {
        qCritical() << "Error processing request " << reply->request().url() << " Error:" << reply->error();
    }
    if (!reply->isFinished()) {
        return;
    }

    auto processReply = [ this, buf = reply->readAll() ]() noexcept
    {
        QJsonParseError err{};
        QJsonDocument json = QJsonDocument::fromJson(buf, &err);
        if (json.isNull()) {
            qCritical() << "Json parse error:" << err.errorString();
            return;
        }
        auto location = extractLocationFromJSONDocument(json);
        m_locationsToLoad.remove(location.uuid);
        m_loadedLocations.insert(location.uuid, location);
        m_locationsToInsert.push_back(location);
        emit locationAvailable(location.uuid, location);
        if (m_locationsToLoad.isEmpty()) {
            qDebug() << "ALL LOCATIONS LOADED!";
            m_db->insertLocations(m_locationsToInsert);
            m_locationsToInsert.clear();
        }
    };
    reply->close();
    reply->deleteLater();
    processReply();
}

void LocationProvider::processBatchReply(QNetworkReply *reply)
{
    qDebug() << __PRETTY_FUNCTION__;
    if (reply == nullptr) {
        return;
    }

    qDebug() << "reply.url" << reply->url();

    if (reply->url().toString() != m_locationsInCity.arg(m_countryCode, m_city)) {
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        qCritical() << "Error processing request " << reply->request().url() << " Error:" << reply->error();
    }
    if (!reply->isFinished()) {
        return;
    }

    auto processReply = [ this, buf = reply->readAll() ]() noexcept
    {
        QJsonParseError err{};
        QJsonDocument json = QJsonDocument::fromJson(buf, &err);
        if (json.isNull()) {
            qCritical() << "Json parse error:" << err.errorString();
            return;
        }
        auto rootObj = json.object();
        auto resultIter = rootObj.constFind(QStringLiteral("result"));
        if (resultIter != rootObj.constEnd()) {
            const auto &result = resultIter->toObject();
            qDebug() << "Got " << result.size() << " locations!";
            for (const auto &locationValue : result) {
                const auto &locationObj = locationValue.toObject();
                if (locationObj.isEmpty()) {
                    continue;
                }
                auto location = extractLocationFromJSONObject(locationObj);
                qDebug() << "Got location: uuid=" << location.uuid << ", " << location.toString();
                m_locationsToLoad.remove(location.uuid);
                m_loadedLocations.insert(location.uuid, location);
                m_locationsToInsert.push_back(location);
                emit locationAvailable(location.uuid, location);
            }
        }
        m_db->insertLocations(m_locationsToInsert);
        m_locationsToInsert.clear();
        if (m_locationsToLoad.isEmpty()) {
            qDebug() << "ALL LOCATIONS LOADED!";
        } else {
            qDebug() << "Some locations are remained to be loaded.";
            loadAllLocations();
        }
    };
    reply->close();
    reply->deleteLater();
    processReply();
}

void LocationProvider::setNetworkAccessManager(QNetworkAccessManager *networkAccessManager)
{
    if (m_networkAccessManager) {
        disconnect(m_networkAccessManager, &QNetworkAccessManager::finished, this, &LocationProvider::processFinishedReply);
    }
    m_networkAccessManager = networkAccessManager;
    connect(m_networkAccessManager, &QNetworkAccessManager::finished, this, &LocationProvider::processFinishedReply,
            Qt::ConnectionType::UniqueConnection);
}

void LocationProvider::requestLocation(const QUuid &uuid)
{
    const auto foundIt = m_loadedLocations.constFind(uuid);
    if (foundIt != m_loadedLocations.constEnd()) {
        qDebug() << "LocationProvider: Already loaded!";
        emit locationAvailable(uuid, foundIt.value());
        return;
    }
    Location location;
    bool found;
    std::tie(found, location) = m_db->findLocation(uuid);
    if (found) {
        qDebug() << "LocationProvider: found in DB!";
        m_loadedLocations.insert(uuid, location);
        emit locationAvailable(uuid, location);
        return;
    }
    qDebug() << "LocationProvider: Download needed!";
    auto iter = m_locationsToLoad.constFind(uuid);
    if (iter != m_locationsToLoad.constEnd()) {
        qDebug() << "LocationProvider: Already in load queue!";
        return;
    }
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

void LocationProvider::setLocationsToLoad(QSet< QUuid > &&locations, const QString &countryCode, const QString &city)
{
    qDebug() << __PRETTY_FUNCTION__ << "locations.size() = " << locations.size();
    m_countryCode = countryCode;
    m_city = city;
    m_locationsToInsert.clear();
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
    const auto &locationsForCity = m_db->getLocations(m_countryCode, m_city);
    for (const auto &location : qAsConst(locationsForCity)) {
        m_loadedLocations.insert(location.uuid, location);
    }
    m_locationsToLoad.subtract(m_db->getAllUUIDs());
    m_locationsToInsert.reserve(m_locationsToLoad.size());
    if (!m_locationsToLoad.empty()) {
        QUrl requestUrl(m_locationsInCity.arg(m_countryCode, m_city));
        QNetworkRequest request(requestUrl);
        request.setRawHeader(QByteArrayLiteral("User-Agent"), QByteArrayLiteral("Radar App 1.0"));
        qDebug() << "[Network] Requesting locations for city:" << requestUrl;
        auto reply = m_networkAccessManager->get(request);
        connect(reply, &QNetworkReply::finished, this, [ this, reply ]() noexcept { processBatchReply(reply); });
    }
    //loadAllLocations();
}

QNetworkReply *LocationProvider::requestLocationByUUID(const QUuid &id)
{
    QUrl requestUrl(QStringLiteral("%1%2.json?fields=address,direction,map,title,uuid").arg(m_locationUrlBase, id.toString(QUuid::WithoutBraces)));
    QNetworkRequest request(requestUrl);
    request.setRawHeader(QByteArrayLiteral("User-Agent"), QByteArrayLiteral("Radar App 1.0"));
    qDebug() << "[Network] Requesting location " << id;
    return m_networkAccessManager->get(request);
}

void LocationProvider::doLoad(const QUuid &id)
{
    // load over network
    requestLocationByUUID(id);
}

void LocationProvider::loadAllLocations()
{
    qDebug() << "Locations remained to load from network=" << m_locationsToLoad.size();
    qDebug() << "Locations to load: " << m_locationsToLoad;
    for (const auto &uuid : qAsConst(m_locationsToLoad)) {
        doLoad(uuid);
    }
}

void LocationProvider::setDB(DB *db)
{
    m_db = db;
}
