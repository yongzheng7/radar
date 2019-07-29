#pragma once
#include <QGeoCoordinate>
#include <QHash>
#include <QObject>
#include <QSet>
#include <QStringBuilder>
#include <QUuid>
#include <tuple>

struct Location {
    QUuid uuid;
    QString name;
    QString country;
    QString locality;

    QString firstName;
    QString lastName;

    int postalCode{0};
    QString thoroughfare;
    QString directions;
    QGeoCoordinate coordinate;

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
    QHash< QUuid, Location > m_loadedLocations;
    QNetworkAccessManager *m_networkAccessManager{nullptr};
    DB *m_db{nullptr};
};
