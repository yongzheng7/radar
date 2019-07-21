#pragma once

#include "locationprovider.h"
#include <QtSql>
#include <QObject>
#include <QSet>
#include <QVector>

class DB : QObject
{
    Q_OBJECT
public:
    explicit DB(QObject *parent = nullptr);
    virtual ~DB();
    QSqlError initDB();
    QVariant insertLocation(const Location &location);
    std::pair< bool, Location > findLocation(QUuid uuid);
    QSet<QUuid> getAllUUIDs();
    QStringList getAllCountries();
    QMap<QString, QStringList> getAllCities();
    QVariant insertCountry(const QString &code, const QString &name, const QStringList &cities);
};
