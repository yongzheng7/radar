#include "eventsmodel.h"
#include "locationprovider.h"
#include <tuple>

#include <QDateTime>
#include <QSize>

EventsModel::EventsModel(LocationProvider *locationProvider, QObject *parent)
    : QAbstractListModel(parent)
    , m_locationProvider(locationProvider)
{
    Q_ASSERT(locationProvider);
    connect(m_locationProvider, &LocationProvider::locationAvailable, this, &EventsModel::emitLocationDataChanged,
            Qt::QueuedConnection);
}

EventsModel::~EventsModel()
{
}

void EventsModel::setEvents(QVector< Event > &&events)
{
    beginResetModel();
    m_events = events;
    endResetModel();
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
    default:
        return {};
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
                                          {Roles::LocationName, QByteArrayLiteral("locationName")}};
    return names;
}

void EventsModel::emitLocationDataChanged(const QUuid &uuid)
{
    const auto count = m_events.size();
    for (int row = 0; row < count; ++row) {
        if (m_events[row].locationID == uuid) {
            auto idx = index(row);
            emit dataChanged(idx, idx, {Roles::LocationName});
        }
    }
}
