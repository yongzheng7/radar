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
import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12

import org.radar.app 1.0

Item {
    id: root
    anchors.fill: parent
    anchors.margins: 12
    Column {
        spacing: 10
        anchors.fill: parent
        Text {
            id: countryLabel
            text: qsTr("Country")
        }
        ComboBox {
            id: countries
            width: parent.width
            model: App.countries

            onActivated: {
                App.country = model[index];
            }
            onModelChanged: {
                countries.currentIndex = find(App.country);
            }
        }

        Text {
            id: cityLabel
            text: qsTr("City")
        }
        ComboBox {
            id: cities
            font.capitalization: Font.Capitalize
            width: parent.width
            model: App.cities
            delegate: ItemDelegate {
                text: modelData
                width: parent.width
                font.capitalization: Font.Capitalize
            }

            onActivated: {
                App.city = model[index];
            }

            onModelChanged: {
                cities.currentIndex = find(App.city);
            }
        }
        CheckBox {
            id: remember
            checked: App.isRememberLocationOn
            text: qsTr("Remember location")
            onToggled: App.toggleRememberLocation()
        }
        Text {
            text: qsTr("%1 events in this area".arg(App.totalFoundEvents))
        }
        Text {
            text: qsTr("%1 events today".arg(App.todayFoundEvents))
        }
    }
}
