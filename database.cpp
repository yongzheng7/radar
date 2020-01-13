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
#include "database.h"
#include "locationprovider.h"
#include <tuple>

QVariant DB::insertLocation(const Location &location)
{
    qDebug() << "Storing location wit uuid=" << location.uuid.toString();
    QSqlQuery q;
    q.prepare(QStringLiteral("insert or replace into locations("
                             "uuid, name, country, locality, firstName, "
                             "lastName, postalCode, thoroughfare, "
                             "directions, latitude, longitude"
                             ") values("
                             "?, ?, ?, ?, ?, ?, ?, ?, ?, ? ,?"
                             ")"));
    q.addBindValue(location.uuid.toRfc4122());
    q.addBindValue(location.name);
    q.addBindValue(location.country);
    q.addBindValue(location.locality);
    q.addBindValue(location.firstName);
    q.addBindValue(location.lastName);
    q.addBindValue(location.postalCode);
    q.addBindValue(location.thoroughfare);
    q.addBindValue(location.directions);
    if (location.coordinate.isValid()) {
        q.addBindValue(location.coordinate.latitude());
        q.addBindValue(location.coordinate.longitude());
    } else {
        q.addBindValue(QVariant(QVariant::Double));
        q.addBindValue(QVariant(QVariant::Double));
    }
    q.exec();
    return q.lastInsertId();
}

QVariant DB::insertLocations(const QVector< Location > &locations)
{
    if (locations.empty()) {
        return {};
    }
    QSqlQuery q;
    bool transactionIsRunning = QSqlDatabase::database().transaction();
    q.prepare(QStringLiteral("insert or replace into locations("
                             "uuid, name, country, locality, firstName, "
                             "lastName, postalCode, thoroughfare, "
                             "directions, latitude, longitude"
                             ") values("
                             "?, ?, ?, ?, ?, ?, ?, ?, ?, ? ,?"
                             ")"));
    for (const auto &location : qAsConst(locations)) {
        q.addBindValue(location.uuid.toRfc4122());
        q.addBindValue(location.name);
        q.addBindValue(location.country);
        q.addBindValue(location.locality);
        q.addBindValue(location.firstName);
        q.addBindValue(location.lastName);
        q.addBindValue(location.postalCode);
        q.addBindValue(location.thoroughfare);
        q.addBindValue(location.directions);
        if (location.coordinate.isValid()) {
            q.addBindValue(location.coordinate.latitude());
            q.addBindValue(location.coordinate.longitude());
        } else {
            q.addBindValue(QVariant(QVariant::Double));
            q.addBindValue(QVariant(QVariant::Double));
        }
        q.exec();
    }
    if (transactionIsRunning) {
        QSqlDatabase::database().commit();
    }
    return q.lastInsertId();
}

std::pair< bool, Location > DB::findLocation(QUuid uuid)
{
    qDebug() << "DB: Loading location by uuid=" << uuid.toString(QUuid::WithoutBraces);
    QSqlQuery q;
    q.prepare(QStringLiteral("select name, country, locality, firstName,"
                             " lastName, postalCode, thoroughfare, directions,"
                             " latitude, longitude from locations where uuid = :uuid"));
    q.bindValue(QStringLiteral(":uuid"), uuid.toRfc4122());
    if (q.exec()) {
        if (q.next()) {
            Location result{};
            result.name = q.value(0).toString();
            result.country = q.value(1).toString();
            result.locality = q.value(2).toString();
            result.firstName = q.value(3).toString();
            result.lastName = q.value(4).toString();
            result.postalCode = q.value(5).toInt();
            result.thoroughfare = q.value(6).toString();
            result.directions = q.value(7).toString();
            if (!q.isNull(8) && !q.isNull(9)) {
                qreal latitude = q.value(8).toDouble();
                qreal longitude = q.value(9).toDouble();
                result.coordinate = QGeoCoordinate(latitude, longitude);
            }
            result.uuid = uuid;
            qDebug() << "found location with name " << result.name;
            return {true, result};
        }
    }
    qDebug() << "Location not found!";
    return {false, {}};
}

QVector< Location > DB::getLocations(const QString &countryCode, const QString &city)
{
    QVector< Location > results;
    qDebug() << "DB: Loading all locations location for " << countryCode << "/" << city;
    QSqlQuery q;
    q.prepare(QStringLiteral("select uuid, name, firstName,"
                             " lastName, postalCode, thoroughfare, directions,"
                             " latitude, longitude from locations where country = :country AND locality = :city"));
    q.bindValue(QStringLiteral(":country"), countryCode);
    q.bindValue(QStringLiteral(":city"), city);
    if (q.exec()) {
        if (q.size() > 0) {
            results.reserve(q.size());
        }
        while (q.next()) {
            Location result{};
            result.uuid = QUuid::fromRfc4122(q.value(0).toByteArray());//
            result.name = q.value(1).toString();
            result.country = countryCode;
            result.locality = city;
            result.firstName = q.value(2).toString();
            result.lastName = q.value(3).toString();
            result.postalCode = q.value(4).toInt();
            result.thoroughfare = q.value(5).toString();
            result.directions = q.value(6).toString();
            if (!q.isNull(7) && !q.isNull(8)) {
                qreal latitude = q.value(7).toDouble();
                qreal longitude = q.value(8).toDouble();
                result.coordinate = QGeoCoordinate(latitude, longitude);
            }
            results.push_back(std::move(result));
        }
    }
    qDebug() << "Loaded " << results.size() << " entries for " << city;
    return results;
}

QSet< QUuid > DB::getAllUUIDs()
{
    QSet< QUuid > retval;
    qDebug() << "Loading available uuids...";
    QSqlQuery q;
    q.prepare(QStringLiteral("select uuid from locations"));

    if (q.exec()) {
        retval.reserve(q.size());
        while (q.next()) {
            retval.insert(QUuid::fromRfc4122(q.value(0).toByteArray()));
        }
    } else {
        qCritical() << "Error selecting location uuids:" << q.lastError().text();
    }
    qDebug() << "Selected " << retval.size() << " UUIDs!";
    return retval;
}

QStringList DB::getAllCountries()
{
    QSqlQuery q;
    q.prepare(QStringLiteral("SELECT name FROM countries"));
    if (q.exec()) {
        QStringList result;
        result.reserve(q.size());
        while (q.next()) {
            result.push_back(q.value(0).toString());
        }
        return result;
    }
    qCritical() << "Failed to SELECT all countries:" << q.lastError();
    return {};
}

QMap< QString, QStringList > DB::getAllCities()
{
    QSqlQuery q;
    q.prepare(QStringLiteral(
        "select code, cities.name from countries, cities where countries.id = cities.countryid ORDER BY code,cities.name"));
    if (q.exec()) {
        QMap< QString, QStringList > result;
        while (q.next()) {
            result[q.value(0).toString()].push_back(q.value(1).toString());
        }
        return result;
    }
    qCritical() << "Failed to SELECT all cities:" << q.lastError();
    return {};
}

QVariant DB::insertCountry(const QString &code, const QString &name, const QStringList &cities)
{
    QSqlQuery q;
    bool transaction = QSqlDatabase::database().transaction();
    q.prepare(QStringLiteral("INSERT OR REPLACE INTO countries(code, name) VALUES(:code, :name)"));
    q.bindValue(QStringLiteral(":code"), code);
    q.bindValue(QStringLiteral(":name"), name);
    q.exec();
    if (!q.lastInsertId().isValid()) {
        qCritical() << "Failed to insert into countries:" << q.lastError().text();
        if (transaction) {
            QSqlDatabase::database().rollback();
        }
        return {};
    }
    int countryId = q.lastInsertId().toInt();

    q.prepare(QStringLiteral("INSERT OR REPLACE INTO cities(countryId, name) VALUES(:countryId, :name)"));
    for (const auto &city : qAsConst(cities)) {
        q.bindValue(QStringLiteral(":countryId"), countryId);
        q.bindValue(QStringLiteral(":name"), city);
        q.exec();
        Q_ASSERT(q.lastInsertId().isValid());
    }
    if (transaction) {
        bool commited = QSqlDatabase::database().commit();
        qDebug() << "insertCountry() transaction commited=" << commited;
    }
    return q.lastInsertId();
}

void DB::clearCountries()
{
    qDebug() << __PRETTY_FUNCTION__;
    QSqlQuery q;
    q.prepare(QStringLiteral("delete from countries"));
    if (q.exec()) {
        qDebug() << "success";
    }
    qCritical() << "Error deleting countries:" << q.lastError().text();
}

void DB::clearCities()
{
    qDebug() << __PRETTY_FUNCTION__;
    QSqlQuery q;
    q.prepare(QStringLiteral("delete from cities"));
    if (q.exec()) {
        qDebug() << "success";
    }
    qCritical() << "Error deleting cities:" << q.lastError().text();

}

DB::DB(QObject *parent)
    : QObject(parent)
{
}

DB::~DB() = default;

QSqlError DB::initDB()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    QFile asset(QStringLiteral(":/db.sqlite"));
    if (!asset.exists()) {
        qCritical() << "No DB file in assets!";
        return QSqlError();
    }
    const auto dir = QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    if (!dir.mkpath(dir.absolutePath())) {
        qCritical() << "Can not create dir " << dir.absolutePath();
        return QSqlError();
    }
    const auto targetPath = dir.filePath(QStringLiteral("radar-app.db"));
    qDebug() << "DB file path: " << targetPath;
    if (!QFile::exists(targetPath)) {
        qDebug() << "copying DB from assets...";
        if (!dir.exists(targetPath) && !asset.copy(targetPath)) {
            qCritical() << "Can not copy to " << targetPath;
            return QSqlError();
        }
        QFile(targetPath).setPermissions(QFileDevice::Permission::ReadOwner | QFileDevice::Permission::WriteOwner);
    }
    db.setDatabaseName(targetPath);

    if (!db.open()) {
        return db.lastError();
    }

    QSqlQuery q;
    QStringList tables = db.tables();
    if (tables.contains(QStringLiteral("locations"), Qt::CaseInsensitive)) {
        qCritical() << "table LOCATIONS already exists!";
    } else {
        if (!q.exec(QStringLiteral("create table locations("
                                   " uuid varchar primary key, name varchar,"
                                   " country varchar,"
                                   " locality varchar, firstName varchar,"
                                   " lastName varchar, postalCode integer,"
                                   " thoroughfare varchar, directions varchar,"
                                   " latitude varchar, longitude varchar)"))) {
            return q.lastError();
        }
    }

    if (tables.contains(QStringLiteral("countries"), Qt::CaseInsensitive)) {
        qCritical() << "table COUNTRIES already exists!";
    } else {
        if (!q.exec(QStringLiteral("CREATE TABLE countries(id integer primary key,"
                                   " code varchar, name varchar)"))) {
            return q.lastError();
        }
    }
    if (tables.contains(QStringLiteral("cities"), Qt::CaseInsensitive)) {
        qCritical() << "table COUNTRIES already exists!";
    } else {
        if (!q.exec(QStringLiteral("CREATE TABLE cities(id INTEGER PRIMARY KEY,"
                                   "    countryId VARCHAR,"
                                   "    name varchar,"
                                   "    FOREIGN KEY(countryId) REFERENCES countries(code)"
                                   ")"))) {
            return q.lastError();
        }
    }
    return QSqlError();
}
