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
#include "eventsmodel.h"
#include "locationprovider.h"
#include <tuple>

#include <QDateTime>
#include <QDebug>
#include <QGeoPositionInfoSource>
#include <QSize>

EventsModel::EventsModel(LocationProvider *locationProvider, QObject *parent)
    : QAbstractListModel(parent)
    , m_locationProvider(locationProvider)
    , m_positionSource(QGeoPositionInfoSource::createDefaultSource(this))
{
    Q_ASSERT(locationProvider);
    connect(m_locationProvider, &LocationProvider::locationAvailable, this, &EventsModel::emitLocationDataChanged,
            Qt::QueuedConnection);
    qDebug() << "m_positionSource =" << m_positionSource;
    if (m_positionSource) {
        connect(m_positionSource, QOverload< QGeoPositionInfoSource::Error >::of(&QGeoPositionInfoSource::error),
                this, [this](QGeoPositionInfoSource::Error positioningError) noexcept {
                    m_geoError = positioningError;
                    emit this->hasGeoErrorChanged(QPrivateSignal());
                });
        constexpr auto updateIntervalMSec = 5000;
        m_positionSource->setUpdateInterval(updateIntervalMSec);
        connect(m_positionSource, &QGeoPositionInfoSource::positionUpdated,
                this, [this](const QGeoPositionInfo &newPos) noexcept {
                    qDebug() << "got new GeoPositionInfo!";
                    if (m_latestPosition.isValid() && m_latestPosition.distanceTo(newPos.coordinate()) - 1.0 <= 1.0) {
                        return;
                    }
                    m_latestPosition = newPos.coordinate();
                    const auto indexFirst = index(0);
                    const auto indexLast = index(m_events.size() - 1);
                    emit dataChanged(indexFirst, indexLast, {Roles::Distance});
                });
    }
}

EventsModel::~EventsModel() = default;

void EventsModel::setEvents(QVector< Event > &&events)
{
    beginResetModel();
    m_events = events;
    endResetModel();
}

void EventsModel::appendEvents(QVector< Event > &&events)
{
    beginInsertRows(QModelIndex(), m_events.size(), m_events.size() + events.size() - 1);
    m_events.append(events);
    endInsertRows();
}

void EventsModel::setEvents(const QVector< Event > &events)
{
    beginResetModel();
    m_events = events;
    endResetModel();
}

Event EventsModel::getEventByIndex(int index) const
{
    Q_ASSERT(index >= 0 && index < m_events.size());
    return m_events.value(index);
}

int EventsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_events.size();
}

int EventsModel::todayEventsCount() const
{
    QDate today = QDate::currentDate();
    return static_cast< int >(std::count_if(
        m_events.cbegin(), m_events.cend(), [today](const Event &event) noexcept { return event.date == today; }));
}

int EventsModel::todaysFirstEventIndex() const
{
    const auto constBegin = m_events.cbegin();
    const auto constEnd = m_events.cend();
    const QDate today = QDate::currentDate();
    const auto foundIter
        = std::find_if(constBegin, constEnd, [&today](const Event &event) noexcept { return event.date == today; });
    if (foundIter == constEnd) {
        return -1;
    }
    return std::distance(constBegin, foundIter);
}

namespace
{
    QString prettyDistance(double meters)
    {
        constexpr auto one_km = 1000.0;
        if (meters < one_km) {
            return QObject::tr("%1 m").arg(qRound(meters));
        }
        return QObject::tr("%1 km").arg(QString::number(meters / one_km, 'f', 1));
    }
}// namespace
QVariant EventsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    const int pos = index.row();
    if (pos < 0 || pos >= m_events.count()) {
        return {};
    }
    switch (role) {
    case Title:
        return m_events[pos].title;
    case Address:
        return m_events[pos].rawAddress;
    case Date:
        return m_events[pos].date.toString(Qt::DateFormat::DefaultLocaleLongDate);
    case StartDateTime:
        return QDateTime::fromSecsSinceEpoch(m_events[pos].timeStart).toString(QStringLiteral("hh:mm"));
    case Category:
        return m_events[pos].category;
    case LocationName: {
        Location location;
        bool exists;
        std::tie(location, exists) = m_locationProvider->getLoadedLocation(m_events[pos].locationID);
        if (!exists) {
            m_locationProvider->requestLocation(m_events[pos].locationID);
            return QString();
        }
        return location.name;
    }
    case Distance: {
        if (!m_latestPosition.isValid()) {
            return QString();
        }
        Location location;
        bool exists;
        std::tie(location, exists) = m_locationProvider->getLoadedLocation(m_events[pos].locationID);
        if (!exists) {
            m_locationProvider->requestLocation(m_events[pos].locationID);
            return QString();
        }
        if (!location.coordinate.isValid()) {
            return QString();
        }
        auto distanceInMeters = m_latestPosition.distanceTo(location.coordinate);
        return prettyDistance(distanceInMeters);
    }
    case Url:
        return m_events[pos].radarUrl;
    default:
        return QString();
    }
}

QHash< int, QByteArray > EventsModel::roleNames() const
{
    static QHash< int, QByteArray > names{{Roles::Title, QByteArrayLiteral("title")},
                                          {Roles::Place, QByteArrayLiteral("place")},
                                          {Roles::Address, QByteArrayLiteral("address")},
                                          {Roles::Date, QByteArrayLiteral("date")},
                                          {Roles::StartDateTime, QByteArrayLiteral("startDateTime")},
                                          {Roles::Category, QByteArrayLiteral("category")},
                                          {Roles::LocationName, QByteArrayLiteral("locationName")},
                                          {Roles::Distance, QByteArrayLiteral("distance")},
                                          {Roles::Url, QByteArrayLiteral("url")}};
    return names;
}

void EventsModel::forceUpdatePosition()
{
    if (m_positionSource) {
        m_positionSource->requestUpdate();
    }
}

void EventsModel::stopUpdatePosition()
{
    if (m_positionSource) {
        m_positionSource->stopUpdates();
    }
}

void EventsModel::startUpdatePosition()
{
    if (m_positionSource) {
        m_positionSource->startUpdates();
    }
}

bool EventsModel::hasGeoError() const
{
    return !m_positionSource || m_positionSource->error() != QGeoPositionInfoSource::Error::NoError;
}

void EventsModel::emitLocationDataChanged(const QUuid &uuid)
{
    Q_UNUSED(uuid);
    const auto indexFirst = index(0);
    const auto indexLast = index(m_events.size() - 1);
    emit dataChanged(indexFirst, indexLast, {Roles::LocationName, Roles::Distance});
}
