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

#include <QAbstractListModel>
#include <QDate>
#include <QGeoCoordinate>
#include <QGeoPositionInfoSource>
#include <QHash>
#include <QUuid>

struct Event {
    QString title;
    QString description;
    QUuid locationID;
    QString rawAddress;
    QDate date;
    qint64 timeStart{};
    qint64 timeEnd{};
    QString price;
    QString category;
    QString radarUrl;
};

Q_DECLARE_METATYPE(Event)

class LocationProvider;

class EventsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    EventsModel(LocationProvider *locationProvider, QObject *parent = nullptr);
    ~EventsModel() override;

    enum Roles { Title = Qt::UserRole, Place, Address, Date, StartDateTime, Category, LocationName, Distance, Url };

public:
    void setEvents(QVector< Event > &&events);
    void setEvents(const QVector< Event > &events);

    Event getEventByIndex(int index) const;
    int rowCount(const QModelIndex &parent) const override;
    int todayEventsCount() const;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash< int, QByteArray > roleNames() const override;

    void forceUpdatePosition();
    void stopUpdatePosition();
    void startUpdatePosition();
    bool hasGeoError() const;

signals:
    void hasGeoErrorChanged(QPrivateSignal);

private:
    void emitLocationDataChanged(const QUuid &uuid);

private:
    Q_DISABLE_COPY(EventsModel)
    QVector< Event > m_events;
    LocationProvider *const m_locationProvider;
    QGeoPositionInfoSource *const m_positionSource;
    QGeoCoordinate m_latestPosition;
    QGeoPositionInfoSource::Error m_geoError{QGeoPositionInfoSource::NoError};
};

Q_DECLARE_METATYPE(QAbstractItemModel *);
