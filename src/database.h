/*
 *   database.h
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

#include "locationprovider.h"
#include <QObject>
#include <QSet>
#include <QVector>
#include <QtSql>

#include <utility>

class DB : QObject
{
    Q_OBJECT
public:
    explicit DB(QObject *parent = nullptr);
    ~DB() override;
    QSqlError initDB();
    QVariant insertLocation(const Location &location);
    QVariant insertLocations(const QVector< Location > &locations);
    std::pair< bool, Location > findLocation(QUuid uuid);
    QVector< Location > getLocations(const QString &countryCode, const QString &city);
    QSet< QUuid > getAllUUIDs();
    QStringList getAllCountries();
    QMap< QString, QStringList > getAllCities();
    QVariant insertCountry(const QString &code, const QString &name, const QStringList &cities);
    void clearCountries();
    void clearCities();

private:
    Q_DISABLE_COPY(DB)
};
