#include "eventsmodel.h"
#include <tuple>

#include <QDateTime>
#include <QSize>

EventsModel::EventsModel(QObject *parent)
    : QAbstractListModel(parent)
{
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
//    case Qt::SizeHintRole:
//        return QSize(400, 60);// FIXME
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
                                          {Roles::Category, QByteArrayLiteral("category")}};
    return names;
}
