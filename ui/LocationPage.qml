/*
 *   LocationPage.qml
 *
 *   Copyright (c) 2021 Andy Ex
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
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import org.radar.app 1.0

import "Icon.js" as MdiFont

Item {
    id: root

    readonly property bool currentOSIsAndroid: Qt.platform.os === "android"

    anchors.fill: parent
    anchors.margins: 12

    signal showClicked()

    Column {
        spacing: 10

        width: root.width - root.anchors.margins*2
        anchors.centerIn: parent

        GridLayout {
            columns: 2
            rowSpacing: 10
            columnSpacing: 10

            width: parent.width

            Label {
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

                Material.elevation: 0
            }

            Label {
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
                Material.elevation: 0
            }

            CheckBox {
                id: remember
                Layout.row: 3
                Layout.column: 1
                Layout.fillWidth: true
                checked: App.isRememberLocationOn
                text: qsTr("Remember location")
                onToggled: App.toggleRememberLocation()
            }
            ColumnLayout {
                spacing: 10

                Layout.row: 4
                Layout.column: 1
                Layout.rowSpan: 2

                Layout.fillWidth: true

                Label {
                    wrapMode: Text.WordWrap

                    text: qsTr("%1 events in this area").arg(App.totalFoundEvents)
                    Layout.fillWidth: true
                }

                Label {
                    wrapMode: Text.WordWrap

                    text: qsTr("%1 events today").arg(App.todayFoundEvents)
                    Layout.fillWidth: true
                }

                property int buttonWidth: Math.max(showEvents.implicitWidth, reload.implicitWidth)

                IconButton {
                    id: reload

                    iconCode: MdiFont.Icon.reload
                    text: qsTr("Reload...")

                    onActivated: {
                        App.resetNetworkConnection();
                        App.reloadEvents();
                    }
                    enabled: App.state === AppStates.Idle
                    Layout.preferredWidth: parent.buttonWidth
                    Layout.alignment: Qt.AlignRight
                }
                Button {
                    id: showEvents

                    text: root.currentOSIsAndroid ?
                              qsTr("Show >") :
                              qsTr("Show...")

                    onClicked: root.showClicked()

                    enabled: App.state === AppStates.Idle && App.totalFoundEvents > 0

                    Layout.preferredWidth: parent.buttonWidth
                    Layout.alignment: Qt.AlignRight
                }
            }
        }
    }
}
