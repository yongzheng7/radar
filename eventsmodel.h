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

class EventsModel : public QAbstractListModel
{
public:
    EventsModel(QObject *parent = nullptr);
    ~EventsModel() override;

    enum Roles { Title = Qt::UserRole, Place, Address, Date, StartDateTime, Category };

public:
    void setEvents(QVector< Event > &&events);
    void setEvents(const QVector< Event > &events);

    Event getEventByIndex(int index) const;
    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash< int, QByteArray > roleNames() const override;

private:
    Q_DISABLE_COPY(EventsModel)
    QVector< Event > m_events;

};

Q_DECLARE_METATYPE(QAbstractItemModel *);
