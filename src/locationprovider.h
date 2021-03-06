/*
 *   locationprovider.h
 *
 *   Copyright (c) 2021 Andy Ex
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
#include <QGeoCoordinate>
#include <QHash>
#include <QObject>
#include <QSet>
#include <QStringBuilder>
#include <QUuid>
#include <QVector>
#include <utility>

struct Location {
    QUuid uuid;
    QString name;
    QString country;
    QString locality;

    QString firstName;
    QString lastName;

    QString thoroughfare;
    QString directions;
    QGeoCoordinate coordinate;
    int postalCode{0};

    QString toString() const;
};

Q_DECLARE_METATYPE(Location)

class DB;
class QNetworkReply;
class QNetworkAccessManager;
class LocationProvider : public QObject
{
    Q_OBJECT
public:
    LocationProvider(QNetworkAccessManager &accessManager, QObject *parent = nullptr);
    ~LocationProvider() override;

    void requestLocation(const QUuid &uuid);
    std::pair< Location, bool > getLoadedLocation(const QUuid &uuid) const;
    void clearLoadedLocations();

    void setCity(const QString &city);
    void setLocationFromEvents(QHash< QUuid, Location > &&locations);

    void setLocationsToLoad(QSet< QUuid > &&locations, const QString &countryCode, const QString &city);
    void doLoad(const QUuid &id);
    void loadAllLocations();
    void setDB(DB *db);
    static Location extractLocationFromJSONObject(const QJsonObject &obj);

private:
    Q_DISABLE_COPY(LocationProvider)
    void processFinishedReply(QNetworkReply *reply);
    void processBatchReply(QNetworkReply *reply);
    QNetworkReply *requestLocationByUUID(const QUuid &id);
    Location extractLocationFromJSONDocument(const QJsonDocument &json);

signals:
    void locationAvailable(const QUuid &id, const Location &location);

private:
    QSet< QUuid > m_locationsToLoad;
    QVector< Location > m_locationsToInsert;
    QHash< QUuid, Location > m_loadedLocations;
    QNetworkAccessManager &m_networkAccessManager;
    DB *m_db{nullptr};
    const QString m_locationUrlBase{QStringLiteral("https://radar.squat.net/api/1.2/location/")};
    const QString m_locationsInCity{
        QStringLiteral("https://radar.squat.net/api/1.2/search/"
                       "location.json?facets[country][]=%1&facets[locality][]=%2&fields=address,directions,map,title,uuid")};
    QString m_countryCode;
    QString m_city;
};
