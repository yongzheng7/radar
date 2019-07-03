#pragma once

#include <QMap>
#include <QString>

namespace Countries
{

    QMap< QString, QString > allCountries();

    QString countryCode(const QString &country);
}// namespace Countries
