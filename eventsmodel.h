#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QDate>
#include <QUuid>

struct Event {
    QString title;
    QString description;
    QUuid locationID; //replace with 16-byte integer
    QString rawAddress;
    QDate date;
    qint64 timeStart;
    qint64 timeEnd;
    QString category;
};

Q_DECLARE_METATYPE(Event)

class LocationProvider;

class EventsModel : public QAbstractListModel
{
public:
    EventsModel(LocationProvider *locationProvider, QObject *parent = nullptr);
    ~EventsModel() override;

    enum Roles { Title = Qt::UserRole, Place, Address, Date, StartDateTime, Category, LocationName };

public:
    void setEvents(QVector< Event > &&events);
    void setEvents(const QVector< Event > &events);

    Event getEventByIndex(int index) const;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash< int, QByteArray > roleNames() const override;

private:
    void emitLocationDataChanged(const QUuid &uuid);
private:
    Q_DISABLE_COPY(EventsModel)
    QVector< Event > m_events;
    LocationProvider * const m_locationProvider;
};

Q_DECLARE_METATYPE(QAbstractItemModel *);
