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
import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12

import org.radar.app 1.0

Item {
    id: root

    anchors.fill: parent
    anchors.margins: 12

    signal showClicked()

    Column {
        spacing: 10
        anchors.fill: parent

        GridLayout {
            columns: 2
            rowSpacing: 10
            columnSpacing: 10
            anchors.left: parent.left
            anchors.right: parent.right

            Text {
                id: countryLabel

                Layout.row: 0
                Layout.column: 0
                Layout.fillWidth: false

                text: qsTr("Country")
            }
            ComboBox {
                id: countries

                Layout.row: 0
                Layout.column: 1
                Layout.fillWidth: true

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

                Layout.row: 1
                Layout.column: 0
                Layout.fillWidth: false

                text: qsTr("City")
            }
            ComboBox {
                id: cities

                Layout.row: 1
                Layout.column: 1
                Layout.fillWidth: true

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
        }
        CheckBox {
            id: remember
            checked: App.isRememberLocationOn
            text: qsTr("Remember location")
            onToggled: App.toggleRememberLocation()
        }
        GridLayout {
            columns: 2
            rowSpacing: 10
            columnSpacing: 10

            anchors.left: parent.left
            anchors.right: parent.right

            Text {
                Layout.row: 0
                Layout.column: 0
                Layout.fillWidth: true

                wrapMode: Text.WordWrap

                text: qsTr("%1 events in this area".arg(App.totalFoundEvents))
            }

            Text {
                Layout.row: 1
                Layout.column: 0
                Layout.fillWidth: true

                wrapMode: Text.WordWrap

                text: qsTr("%1 events today".arg(App.todayFoundEvents))
            }

            Button {
                id: showEvents

                Layout.row: 0
                Layout.column: 1
                Layout.fillWidth: true
                Layout.maximumWidth: Math.max(showEvents.implicitWidth, reload.implicitWidth)

                text: qsTr("Show...")

                onClicked: root.showClicked()

                visible: App.totalFoundEvents > 0
                enabled: App.state === AppStates.Idle
            }

            Button {
                id: reload

                Layout.row: 1
                Layout.column: 1
                Layout.fillWidth: true
                Layout.maximumWidth: Math.max(showEvents.implicitWidth, reload.implicitWidth)

                text: qsTr("Reload...")
                onClicked: {
                    App.resetNetworkConnection();
                    App.reloadEvents();
                }
                enabled: App.state === AppStates.Idle
            }
        }
    }
}
