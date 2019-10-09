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
import QtQuick.Controls.Material 2.12

Rectangle {
    id: root

    readonly property bool currentOSIsAndroid: Qt.platform.os === "android"

    Shortcut {
        sequences:  root.currentOSIsAndroid ? ["Esc", "Back"] : ["Esc"]
        onActivated: root.closeClicked()
        enabled: root.visible
    }

    Material.background: Material.background

    signal linkActivated(string link)
    signal closeClicked()
    signal locationActivated()
    signal addToCalendarClicked()
    signal openUrlRequested()
    signal shareRequested()

    property string title: "Disco!"
    property string description: "<p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras a nibh lorem. Donec in nisi nec nisi feugiat feugiat tincidunt ac libero. Proin maximus a purus vel facilisis. Aenean ac facilisis augue. Sed nec purus mollis, sollicitudin velit a, convallis magna. Sed lacinia ut nisl a facilisis. Nulla facilisi. Sed dapibus dui nec purus lobortis molestie. Duis pretium libero justo, et egestas metus blandit eget. Proin porttitor dolor nec nibh sagittis imperdiet. Curabitur pharetra consequat arcu, ac venenatis erat pulvinar a.
</p><p>
Cras nec ante sit amet augue sodales iaculis. Aliquam erat volutpat. Nam aliquet quis ipsum nec varius. Integer rutrum lacus eu est ullamcorper, bibendum egestas lorem posuere. Aliquam bibendum ligula in convallis bibendum. Donec efficitur, lacus at aliquam feugiat, risus turpis consequat lorem, ut placerat dolor est a magna. Cras lorem erat, malesuada a nunc tempor, gravida elementum dolor. Duis pharetra lobortis sapien. Quisque vulputate diam et magna facilisis, ac auctor dolor consequat. Praesent aliquam nunc est, id egestas risus iaculis ut. Curabitur pharetra enim eu arcu pretium finibus. Suspendisse potenti. Duis semper turpis enim, nec vulputate ligula fringilla pharetra.</p>"

    property string dateTime: "Sonntag, 12. Mai, 14:00"
    property string category: "Party"
    property string price: "free"
    property string locationName: "US Embassy"
    property string locationAddress: "Brandenburger Tor, 1"
    property string directions: "near SBhf Brandenburger Tor"
    property string city: "Berlin"
    property string country: "Germany"
    property bool urlProvided: true

    Keys.onBackPressed: {
        root.closeClicked();
    }

    Keys.onEscapePressed: root.closeClicked()

    Flickable {
        id: flickable
        anchors.fill: parent
        anchors.margins: 6

        contentWidth: parent.width - anchors.margins*2
        contentHeight: column.implicitHeight + closeButton.height + 2*closeButton.anchors.bottomMargin
        clip: true

        ScrollIndicator.vertical: ScrollIndicator {}

        ColumnLayout {
            id: column
            spacing: 12
            width: parent.width

            Text {
                Layout.fillWidth: true
                Layout.fillHeight: true
                id: header
                textFormat: Text.AutoText
                text: root.title
                font.bold: true
                wrapMode: Text.Wrap
            }
            RowLayout {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                height: Layout.preferredHeight
                spacing: 6
                Label {
                    Layout.alignment: Qt.AlignTop

                    text: qsTr("at:")
                    font.bold: true
                }
                TextEdit {
                    Layout.fillWidth: true

                    readOnly: true
                    selectByMouse: true
                    text: root.locationName
                    wrapMode: Text.Wrap
                }
            }
            Text {
                Layout.fillWidth: true
                id: description
                wrapMode: Text.Wrap
                textFormat: Text.RichText
                text: root.description
                onLinkActivated: {
                    root.linkActivated(link);
                }
                Loader {
                    anchors.fill:parent

                    active: Qt.platform.os !== "android"
                    sourceComponent: MouseArea {
                        hoverEnabled: true
                        readonly property string hoveredLink: description.linkAt(mouseX,mouseY)
                        cursorShape: hoveredLink !== "" ? Qt.PointingHandCursor : Qt.ArrowCursor
                        onPressed: {
                            if (hoveredLink !== "") {
                                root.linkActivated(hoveredLink);
                            }
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                height: Layout.preferredHeight
                spacing: 6
                Label {
                    Layout.alignment: Qt.AlignTop
                    text: qsTr("Date & Time:")
                    font.bold: true
                }
                Text {
                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true
                    text: root.dateTime
                    wrapMode: TextEdit.Wrap
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 6
                visible: root.category.length > 0
                Label {
                    Layout.alignment: Qt.AlignTop
                    text: qsTr("Category:")
                    font.bold: true
                }
                Text {
                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true
                    text: root.category
                }
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 6
                Label {
                    Layout.alignment: Qt.AlignTop
                    text: qsTr("Price:")
                    font.bold: true
                }
                Text {
                    Layout.alignment: Qt.AlignTop
                    text: root.price
                    Layout.fillWidth: true
                }
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 6

                Label {
                    Layout.alignment: Qt.AlignTop
                    text: qsTr("Address:")
                    font.bold: true
                }
                TextEdit {
                    Layout.alignment: Qt.AlignTop
                    id: address
                    readOnly: true
                    Layout.fillWidth: true
                    wrapMode: TextEdit.Wrap
                    text: root.locationAddress
                    font.underline: true
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            console.log("Location: %1".arg(address.text));
                            root.locationActivated();
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                visible: root.directions.length > 0
                spacing: 6
                Label {
                    Layout.alignment: Qt.AlignTop
                    text: qsTr("Directions:")
                    font.bold: true
                }

                TextEdit {
                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true
                    readOnly: true
                    text: root.directions
                    wrapMode: TextEdit.Wrap
                }
            }

            Button {
                id: addToCalendar
                text: qsTr("Add to calendar")
                onClicked: root.addToCalendarClicked()
                Layout.alignment: Qt.AlignHCenter
            }
            Button {
                id: show
                visible: root.urlProvided
                text: qsTr("Show in Browser")
                onClicked: root.openUrlRequested()
                Layout.alignment: Qt.AlignHCenter
            }
            Button {
                id: share
                visible: root.urlProvided
                text: qsTr("Share...")
                onClicked: root.shareRequested()
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
    Button {
        id: closeButton

        z: root.z + 1

        anchors.right:  parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 12
        anchors.rightMargin:  12

        text: qsTr("Close")
        onClicked: root.closeClicked()
    }
}
