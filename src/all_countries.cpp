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
#include "all_countries.h"

#include <QLocale>

#include <QMetaEnum>

QMap< QString, QString > Countries::allCountries()
{
    static QMap< QString, QString > countries{
        {QStringLiteral("Afghanistan"), QStringLiteral("AF")},
        {QStringLiteral("Åland Islands"), QStringLiteral("AX")},
        {QStringLiteral("Albania"), QStringLiteral("AL")},
        {QStringLiteral("Algeria"), QStringLiteral("DZ")},
        {QStringLiteral("American Samoa"), QStringLiteral("AS")},
        {QStringLiteral("Andorra"), QStringLiteral("AD")},
        {QStringLiteral("Angola"), QStringLiteral("AO")},
        {QStringLiteral("Anguilla"), QStringLiteral("AI")},
        {QStringLiteral("Antarctica"), QStringLiteral("AQ")},
        {QStringLiteral("Antigua and Barbuda"), QStringLiteral("AG")},
        {QStringLiteral("Argentina"), QStringLiteral("AR")},
        {QStringLiteral("Armenia"), QStringLiteral("AM")},
        {QStringLiteral("Aruba"), QStringLiteral("AW")},
        {QStringLiteral("Australia"), QStringLiteral("AU")},
        {QStringLiteral("Austria"), QStringLiteral("AT")},
        {QStringLiteral("Azerbaijan"), QStringLiteral("AZ")},
        {QStringLiteral("Bahamas"), QStringLiteral("BS")},
        {QStringLiteral("Bahrain"), QStringLiteral("BH")},
        {QStringLiteral("Bangladesh"), QStringLiteral("BD")},
        {QStringLiteral("Basque"), QStringLiteral("XE")},
        {QStringLiteral("Barbados"), QStringLiteral("BB")},
        {QStringLiteral("Belarus"), QStringLiteral("BY")},
        {QStringLiteral("Belgium"), QStringLiteral("BE")},
        {QStringLiteral("Belize"), QStringLiteral("BZ")},
        {QStringLiteral("Benin"), QStringLiteral("BJ")},
        {QStringLiteral("Bermuda"), QStringLiteral("BM")},
        {QStringLiteral("Bhutan"), QStringLiteral("BT")},
        {QStringLiteral("Bolivia"), QStringLiteral("BO")},
        {QStringLiteral("Bonaire, Sint Eustatius and Saba"), QStringLiteral("BQ")},
        {QStringLiteral("Bosnia and Herzegovina"), QStringLiteral("BA")},
        {QStringLiteral("Botswana"), QStringLiteral("BW")},
        {QStringLiteral("Bouvet Island"), QStringLiteral("BV")},
        {QStringLiteral("Brazil"), QStringLiteral("BR")},
        {QStringLiteral("British Indian Ocean Territory"), QStringLiteral("IO")},
        {QStringLiteral("Brunei Darussalam"), QStringLiteral("BN")},
        {QStringLiteral("Bulgaria"), QStringLiteral("BG")},
        {QStringLiteral("Burkina Faso"), QStringLiteral("BF")},
        {QStringLiteral("Burundi"), QStringLiteral("BI")},
        {QStringLiteral("Cabo Verde"), QStringLiteral("CV")},
        {QStringLiteral("Cambodia"), QStringLiteral("KH")},
        {QStringLiteral("Cameroon"), QStringLiteral("CM")},
        {QStringLiteral("Canada"), QStringLiteral("CA")},
        {QStringLiteral("Catalonia"), QStringLiteral("XC")},
        {QStringLiteral("Cayman Islands"), QStringLiteral("KY")},
        {QStringLiteral("Central African Republic"), QStringLiteral("CF")},
        {QStringLiteral("Chad"), QStringLiteral("TD")},
        {QStringLiteral("Chile"), QStringLiteral("CL")},
        {QStringLiteral("China"), QStringLiteral("CN")},
        {QStringLiteral("Christmas Island"), QStringLiteral("CX")},
        {QStringLiteral("Cocos (Keeling) Islands"), QStringLiteral("CC")},
        {QStringLiteral("Colombia"), QStringLiteral("CO")},
        {QStringLiteral("Comoros"), QStringLiteral("KM")},
        {QStringLiteral("Congo"), QStringLiteral("CG")},
        {QStringLiteral("Congo, Democratic Republic of the"), QStringLiteral("CD")},
        {QStringLiteral("Cook Islands"), QStringLiteral("CK")},
        {QStringLiteral("Costa Rica"), QStringLiteral("CR")},
        {QStringLiteral("Côte d'Ivoire"), QStringLiteral("CI")},
        {QStringLiteral("Croatia"), QStringLiteral("HR")},
        {QStringLiteral("Cuba"), QStringLiteral("CU")},
        {QStringLiteral("Curaçao"), QStringLiteral("CW")},
        {QStringLiteral("Cyprus"), QStringLiteral("CY")},
        {QStringLiteral("Czechia"), QStringLiteral("CZ")},
        {QStringLiteral("Denmark"), QStringLiteral("DK")},
        {QStringLiteral("Djibouti"), QStringLiteral("DJ")},
        {QStringLiteral("Dominica"), QStringLiteral("DM")},
        {QStringLiteral("Dominican Republic"), QStringLiteral("DO")},
        {QStringLiteral("Ecuador"), QStringLiteral("EC")},
        {QStringLiteral("Egypt"), QStringLiteral("EG")},
        {QStringLiteral("El Salvador"), QStringLiteral("SV")},
        {QStringLiteral("Equatorial Guinea"), QStringLiteral("GQ")},
        {QStringLiteral("Eritrea"), QStringLiteral("ER")},
        {QStringLiteral("Estonia"), QStringLiteral("EE")},
        {QStringLiteral("Eswatini"), QStringLiteral("SZ")},
        {QStringLiteral("Ethiopia"), QStringLiteral("ET")},
        {QStringLiteral("Falkland Islands (Malvinas)"), QStringLiteral("FK")},
        {QStringLiteral("Faroe Islands"), QStringLiteral("FO")},
        {QStringLiteral("Fiji"), QStringLiteral("FJ")},
        {QStringLiteral("Finland"), QStringLiteral("FI")},
        {QStringLiteral("France"), QStringLiteral("FR")},
        {QStringLiteral("French Guiana"), QStringLiteral("GF")},
        {QStringLiteral("French Polynesia"), QStringLiteral("PF")},
        {QStringLiteral("French Southern Territories"), QStringLiteral("TF")},
        {QStringLiteral("Gabon"), QStringLiteral("GA")},
        {QStringLiteral("Gambia"), QStringLiteral("GM")},
        {QStringLiteral("Georgia"), QStringLiteral("GE")},
        {QStringLiteral("Germany"), QStringLiteral("DE")},
        {QStringLiteral("Ghana"), QStringLiteral("GH")},
        {QStringLiteral("Gibraltar"), QStringLiteral("GI")},
        {QStringLiteral("Greece"), QStringLiteral("GR")},
        {QStringLiteral("Greenland"), QStringLiteral("GL")},
        {QStringLiteral("Grenada"), QStringLiteral("GD")},
        {QStringLiteral("Guadeloupe"), QStringLiteral("GP")},
        {QStringLiteral("Guam"), QStringLiteral("GU")},
        {QStringLiteral("Guatemala"), QStringLiteral("GT")},
        {QStringLiteral("Guernsey"), QStringLiteral("GG")},
        {QStringLiteral("Guinea"), QStringLiteral("GN")},
        {QStringLiteral("Guinea-Bissau"), QStringLiteral("GW")},
        {QStringLiteral("Guyana"), QStringLiteral("GY")},
        {QStringLiteral("Haiti"), QStringLiteral("HT")},
        {QStringLiteral("Heard Island and McDonald Islands"), QStringLiteral("HM")},
        {QStringLiteral("Holy See"), QStringLiteral("VA")},
        {QStringLiteral("Honduras"), QStringLiteral("HN")},
        {QStringLiteral("Hong Kong"), QStringLiteral("HK")},
        {QStringLiteral("Hungary"), QStringLiteral("HU")},
        {QStringLiteral("Iceland"), QStringLiteral("IS")},
        {QStringLiteral("India"), QStringLiteral("IN")},
        {QStringLiteral("Indonesia"), QStringLiteral("ID")},
        {QStringLiteral("Iran (Islamic Republic of)"), QStringLiteral("IR")},
        {QStringLiteral("Iraq"), QStringLiteral("IQ")},
        {QStringLiteral("Ireland"), QStringLiteral("IE")},
        {QStringLiteral("Isle of Man"), QStringLiteral("IM")},
        {QStringLiteral("Israel"), QStringLiteral("IL")},
        {QStringLiteral("Italy"), QStringLiteral("IT")},
        {QStringLiteral("Jamaica"), QStringLiteral("JM")},
        {QStringLiteral("Japan"), QStringLiteral("JP")},
        {QStringLiteral("Jersey"), QStringLiteral("JE")},
        {QStringLiteral("Jordan"), QStringLiteral("JO")},
        {QStringLiteral("Kazakhstan"), QStringLiteral("KZ")},
        {QStringLiteral("Kenya"), QStringLiteral("KE")},
        {QStringLiteral("Kiribati"), QStringLiteral("KI")},
        {QStringLiteral("Korea (North)"), QStringLiteral("KP")},
        {QStringLiteral("Korea (South)"), QStringLiteral("KR")},
        {QStringLiteral("Kosovo"), QStringLiteral("XK")},
        {QStringLiteral("Kuwait"), QStringLiteral("KW")},
        {QStringLiteral("Kyrgyzstan"), QStringLiteral("KG")},
        {QStringLiteral("Lao People's Democratic Republic"), QStringLiteral("LA")},
        {QStringLiteral("Latvia"), QStringLiteral("LV")},
        {QStringLiteral("Lebanon"), QStringLiteral("LB")},
        {QStringLiteral("Lesotho"), QStringLiteral("LS")},
        {QStringLiteral("Liberia"), QStringLiteral("LR")},
        {QStringLiteral("Libya"), QStringLiteral("LY")},
        {QStringLiteral("Liechtenstein"), QStringLiteral("LI")},
        {QStringLiteral("Lithuania"), QStringLiteral("LT")},
        {QStringLiteral("Luxembourg"), QStringLiteral("LU")},
        {QStringLiteral("Macao"), QStringLiteral("MO")},
        {QStringLiteral("Madagascar"), QStringLiteral("MG")},
        {QStringLiteral("Malawi"), QStringLiteral("MW")},
        {QStringLiteral("Malaysia"), QStringLiteral("MY")},
        {QStringLiteral("Maldives"), QStringLiteral("MV")},
        {QStringLiteral("Mali"), QStringLiteral("ML")},
        {QStringLiteral("Malta"), QStringLiteral("MT")},
        {QStringLiteral("Marshall Islands"), QStringLiteral("MH")},
        {QStringLiteral("Martinique"), QStringLiteral("MQ")},
        {QStringLiteral("Mauritania"), QStringLiteral("MR")},
        {QStringLiteral("Mauritius"), QStringLiteral("MU")},
        {QStringLiteral("Mayotte"), QStringLiteral("YT")},
        {QStringLiteral("Mexico"), QStringLiteral("MX")},
        {QStringLiteral("Micronesia"), QStringLiteral("FM")},
        {QStringLiteral("Moldova"), QStringLiteral("MD")},
        {QStringLiteral("Monaco"), QStringLiteral("MC")},
        {QStringLiteral("Mongolia"), QStringLiteral("MN")},
        {QStringLiteral("Montenegro"), QStringLiteral("ME")},
        {QStringLiteral("Montserrat"), QStringLiteral("MS")},
        {QStringLiteral("Morocco"), QStringLiteral("MA")},
        {QStringLiteral("Mozambique"), QStringLiteral("MZ")},
        {QStringLiteral("Myanmar"), QStringLiteral("MM")},
        {QStringLiteral("Namibia"), QStringLiteral("NA")},
        {QStringLiteral("Nauru"), QStringLiteral("NR")},
        {QStringLiteral("Nepal"), QStringLiteral("NP")},
        {QStringLiteral("Netherlands"), QStringLiteral("NL")},
        {QStringLiteral("New Caledonia"), QStringLiteral("NC")},
        {QStringLiteral("New Zealand"), QStringLiteral("NZ")},
        {QStringLiteral("Nicaragua"), QStringLiteral("NI")},
        {QStringLiteral("Niger"), QStringLiteral("NE")},
        {QStringLiteral("Nigeria"), QStringLiteral("NG")},
        {QStringLiteral("Niue"), QStringLiteral("NU")},
        {QStringLiteral("Norfolk Island"), QStringLiteral("NF")},
        {QStringLiteral("North Macedonia"), QStringLiteral("MK")},
        {QStringLiteral("Northern Mariana Islands"), QStringLiteral("MP")},
        {QStringLiteral("Norway"), QStringLiteral("NO")},
        {QStringLiteral("Oman"), QStringLiteral("OM")},
        {QStringLiteral("Pakistan"), QStringLiteral("PK")},
        {QStringLiteral("Palau"), QStringLiteral("PW")},
        {QStringLiteral("Palestine, State of"), QStringLiteral("PS")},
        {QStringLiteral("Panama"), QStringLiteral("PA")},
        {QStringLiteral("Papua New Guinea"), QStringLiteral("PG")},
        {QStringLiteral("Paraguay"), QStringLiteral("PY")},
        {QStringLiteral("Peru"), QStringLiteral("PE")},
        {QStringLiteral("Philippines"), QStringLiteral("PH")},
        {QStringLiteral("Pitcairn"), QStringLiteral("PN")},
        {QStringLiteral("Poland"), QStringLiteral("PL")},
        {QStringLiteral("Portugal"), QStringLiteral("PT")},
        {QStringLiteral("Puerto Rico"), QStringLiteral("PR")},
        {QStringLiteral("Qatar"), QStringLiteral("QA")},
        {QStringLiteral("Réunion"), QStringLiteral("RE")},
        {QStringLiteral("Romania"), QStringLiteral("RO")},
        {QStringLiteral("Russia"), QStringLiteral("RU")},
        {QStringLiteral("Rwanda"), QStringLiteral("RW")},
        {QStringLiteral("Saint Barthélemy"), QStringLiteral("BL")},
        {QStringLiteral("Saint Helena, Ascension and Tristan da Cunha"), QStringLiteral("SH")},
        {QStringLiteral("Saint Kitts and Nevis"), QStringLiteral("KN")},
        {QStringLiteral("Saint Lucia"), QStringLiteral("LC")},
        {QStringLiteral("Saint Martin (French part)"), QStringLiteral("MF")},
        {QStringLiteral("Saint Pierre and Miquelon"), QStringLiteral("PM")},
        {QStringLiteral("Saint Vincent and the Grenadines"), QStringLiteral("VC")},
        {QStringLiteral("Samoa"), QStringLiteral("WS")},
        {QStringLiteral("San Marino"), QStringLiteral("SM")},
        {QStringLiteral("Sao Tome and Principe"), QStringLiteral("ST")},
        {QStringLiteral("Saudi Arabia"), QStringLiteral("SA")},
        {QStringLiteral("Senegal"), QStringLiteral("SN")},
        {QStringLiteral("Serbia"), QStringLiteral("RS")},
        {QStringLiteral("Seychelles"), QStringLiteral("SC")},
        {QStringLiteral("Sierra Leone"), QStringLiteral("SL")},
        {QStringLiteral("Singapore"), QStringLiteral("SG")},
        {QStringLiteral("Sint Maarten (Dutch part)"), QStringLiteral("SX")},
        {QStringLiteral("Slovakia"), QStringLiteral("SK")},
        {QStringLiteral("Slovenia"), QStringLiteral("SI")},
        {QStringLiteral("Solomon Islands"), QStringLiteral("SB")},
        {QStringLiteral("Somalia"), QStringLiteral("SO")},
        {QStringLiteral("South Africa"), QStringLiteral("ZA")},
        {QStringLiteral("South Georgia and the South Sandwich Islands"), QStringLiteral("GS")},
        {QStringLiteral("South Sudan"), QStringLiteral("SS")},
        {QStringLiteral("Spain"), QStringLiteral("ES")},
        {QStringLiteral("Sri Lanka"), QStringLiteral("LK")},
        {QStringLiteral("Sudan"), QStringLiteral("SD")},
        {QStringLiteral("Suriname"), QStringLiteral("SR")},
        {QStringLiteral("Svalbard and Jan Mayen"), QStringLiteral("SJ")},
        {QStringLiteral("Sweden"), QStringLiteral("SE")},
        {QStringLiteral("Switzerland"), QStringLiteral("CH")},
        {QStringLiteral("Syrian Arab Republic"), QStringLiteral("SY")},
        {QStringLiteral("Taiwan, Province of China"), QStringLiteral("TW")},
        {QStringLiteral("Tajikistan"), QStringLiteral("TJ")},
        {QStringLiteral("Tanzania, United Republic of"), QStringLiteral("TZ")},
        {QStringLiteral("Thailand"), QStringLiteral("TH")},
        {QStringLiteral("Timor-Leste"), QStringLiteral("TL")},
        {QStringLiteral("Togo"), QStringLiteral("TG")},
        {QStringLiteral("Tokelau"), QStringLiteral("TK")},
        {QStringLiteral("Tonga"), QStringLiteral("TO")},
        {QStringLiteral("Trinidad and Tobago"), QStringLiteral("TT")},
        {QStringLiteral("Tunisia"), QStringLiteral("TN")},
        {QStringLiteral("Turkey"), QStringLiteral("TR")},
        {QStringLiteral("Turkmenistan"), QStringLiteral("TM")},
        {QStringLiteral("Turks and Caicos Islands"), QStringLiteral("TC")},
        {QStringLiteral("Tuvalu"), QStringLiteral("TV")},
        {QStringLiteral("Uganda"), QStringLiteral("UG")},
        {QStringLiteral("Ukraine"), QStringLiteral("UA")},
        {QStringLiteral("United Arab Emirates"), QStringLiteral("AE")},
        {QStringLiteral("United Kingdom"), QStringLiteral("GB")},
        {QStringLiteral("United States of America"), QStringLiteral("US")},
        {QStringLiteral("United States Minor Outlying Islands"), QStringLiteral("UM")},
        {QStringLiteral("Uruguay"), QStringLiteral("UY")},
        {QStringLiteral("Uzbekistan"), QStringLiteral("UZ")},
        {QStringLiteral("Vanuatu"), QStringLiteral("VU")},
        {QStringLiteral("Venezuela"), QStringLiteral("VE")},
        {QStringLiteral("Viet Nam"), QStringLiteral("VN")},
        {QStringLiteral("Virgin Islands (British)"), QStringLiteral("VG")},
        {QStringLiteral("Virgin Islands (U.S.)"), QStringLiteral("VI")},
        {QStringLiteral("Wallis and Futuna"), QStringLiteral("WF")},
        {QStringLiteral("Western Sahara"), QStringLiteral("EH")},
        {QStringLiteral("Yemen"), QStringLiteral("YE")},
        {QStringLiteral("Zambia"), QStringLiteral("ZM")},
        {QStringLiteral("Zimbabwe"), QStringLiteral("ZW")}};
    return countries;
}

QString Countries::countryCode(const QString &country)
{
    return allCountries().value(country, QString());
}

namespace
{
    QHash< QString, QLocale::Country > countryByCodeMap()
    {
        const auto metaEnum = QMetaEnum::fromType< QLocale::Country >();
        QHash< QString, QLocale::Country > result;
        result.reserve(metaEnum.keyCount());
        for (int i = 0, count = metaEnum.keyCount(); i < count; i++) {
            QLocale locale(QLocale::Language::AnyLanguage, static_cast< QLocale::Country >(metaEnum.value(i)));
            const auto code = locale.name().split(QChar('_')).last();
            if (code.isEmpty()) {
                continue;
            }
            result.insert(code, locale.country());
        }
        return result;
    }
}// namespace

QString Countries::countryByCode(const QString &code)
{
    static auto mapping = countryByCodeMap();
    auto foundIt = mapping.constFind(code);
    if (mapping.cend() != foundIt) {
        return QLocale::countryToString(foundIt.value());
    }
    return allCountries().key(code, code);
}
