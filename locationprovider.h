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
#include <QGeoCoordinate>
#include <QHash>
#include <QObject>
#include <QSet>
#include <QStringBuilder>
#include <QUuid>
#include <QVector>
#include <tuple>

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

    QString toString() const
    {
        return QStringLiteral("name: %1, country %2, locality: %3, firstName: %4, lastName: %5,"
                              "postalCode: %6, thoroughfare: %7, directions: %8,"
                              "latitude: %9, longitude: %10")
            .arg(name, country, locality, firstName, lastName)
            .arg(postalCode)
            .arg(thoroughfare, directions, coordinate.toString(QGeoCoordinate::Degrees));
    }
};

Q_DECLARE_METATYPE(Location)

class DB;
class QNetworkReply;
class QNetworkAccessManager;
class LocationProvider : public QObject
{
    Q_OBJECT
public:
    LocationProvider(QObject *parent = nullptr);
    ~LocationProvider() override;

    void setNetworkAccessManager(QNetworkAccessManager *networkAccessManager);
    void requestLocation(const QUuid &uuid);
    std::pair< Location, bool > getLoadedLocation(const QUuid &uuid) const;
    void setLocationsToLoad(QSet< QUuid > &&locations);
    void doLoad(const QUuid &id);
    void loadAllLocations();
    void setDB(DB *db);

private:
    Q_DISABLE_COPY(LocationProvider)
    void processFinishedReply(QNetworkReply *reply);
    QNetworkReply *requestLocationByUUID(const QUuid &id);
    Location extractLocationFromJSON(const QJsonDocument &json);

signals:
    void locationAvailable(const QUuid &id, const Location &location);

private:
    QSet< QUuid > m_locationsToLoad;
    QVector< Location > m_locationsToInsert;
    QHash< QUuid, Location > m_loadedLocations;
    QNetworkAccessManager *m_networkAccessManager{nullptr};
    DB *m_db{nullptr};
    const QString locationUrlBase {QStringLiteral("https://radar.squat.net/api/1.2/location/")};
};
