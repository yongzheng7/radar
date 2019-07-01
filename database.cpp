#include "database.h"
#include "locationprovider.h"
#include <tuple>


QVariant DB::insertLocation(const Location &location)
{
    qDebug() << "Storing location wit uuid=" << location.uuid.toString();
    QSqlQuery q;
    q.prepare(QStringLiteral("insert into locations("
                             "uuid, name, country, locality, firstName, "
                             "lastName, postalCode, thoroughfare, "
                             "directions, latitude, longitude"
                             ") values("
                             "?, ?, ?, ?, ?, ?, ?, ?, ?, ? ,?"
                             ")"));
    q.addBindValue(location.uuid);
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


std::pair< bool, Location > DB::findLocation(QUuid uuid)
{
    qDebug() << "Loading location by uuid=" << uuid.toString();
    QSqlQuery q;
    q.prepare(QStringLiteral("select name, country, locality, firstName,"
                             " lastName, postalCode, thoroughfare, directions,"
                             " latitude, longitude from locations where uuid = :uuid"));
    q.bindValue(QStringLiteral(":uuid"), uuid);
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

QSet<QUuid> DB::getAllUUIDs()
{
    QSet<QUuid> retval;
    qDebug() << "Loading available uuids...";
    QSqlQuery q;
    q.prepare(QStringLiteral("select uuid from locations"));

    if (q.exec()) {
        retval.reserve(q.size());
        while(q.next()) {
            retval.insert(q.value(0).toUuid());
        }
    } else {
        qCritical() << "Error selecting location uuids:" << q.lastError().text();
    }
    qDebug() << "Selected " << retval.size() << " UUIDs!";
    return retval;
}

DB::DB(QObject *parent) : QObject(parent)
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

    QStringList tables = db.tables();
    if (tables.contains(QStringLiteral("locations"), Qt::CaseInsensitive)) {
        qCritical() << "table LOCATIONS already exists!";
        return QSqlError();
    }

    QSqlQuery q;
    if (!q.exec(QStringLiteral("create table locations(id integer primary key,"
                               " uuid varchar, name varchar, country varchar,"
                               " locality varchar, firstName varchar,"
                               " lastName varchar, postalCode integer,"
                               " thoroughfare varchar, directions varchar,"
                               " latitude varchar, longitude varchar)"))) {
        return q.lastError();
    }

    return QSqlError();
}
