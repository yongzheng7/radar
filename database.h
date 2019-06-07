#pragma once

#include "locationprovider.h"
#include <QtSql>

namespace DB
{
    QSqlError initDB();
    QVariant insertLocation(const Location &location);
    std::pair< bool, Location > findLocation(QUuid uuid);
}// namespace DB
