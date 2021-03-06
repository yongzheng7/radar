/*
 *   EventView.qml
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

Pane {
    id: root

    readonly property bool currentOSIsAndroid: Qt.platform.os === "android"
    readonly property real subscriptFontSize: fontPointSize * 0.75

    Shortcut {
        sequences:  root.currentOSIsAndroid ? ["Esc", "Back"] : ["Esc"]
        onActivated: root.closeClicked()
        enabled: root.visible
    }

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

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

    property string plainTextDescription: "Plain text description"
    property string dateTime: "Sonntag, 12. Mai, 14:00"
    property string duration: ""
    property string category: "Party"
    property string price: "free"
    property string locationName: "US Embassy"
    property string locationAddress: "Brandenburger Tor, 1"
    property string directions: "near SBhf Brandenburger Tor"
    property string city: "Berlin"
    property string country: "Germany"
    property bool urlProvided: true
    property bool hasRichText: true

    Keys.onBackPressed: root.closeClicked();
    Keys.onEscapePressed: root.closeClicked()

    function updateLocationInfo() {
        root.locationName = App.locationName;
        root.locationAddress = App.locationAddress;
        root.directions = App.directions;
    }

    function updateEventInfo()
    {
        root.title = App.title;
        root.description = App.description;
        root.plainTextDescription = App.plainTextDescription;
        root.category = App.category;
        root.dateTime = App.dateTime;
        root.duration = App.duration
        root.price = App.price;
        root.locationName = App.locationName;
        root.locationAddress = App.locationAddress;
        root.city = App.eventCity;
        root.country = App.eventCountry;
        root.directions = App.directions;
        root.urlProvided = App.url !== "";
        root.hasRichText = App.hasRichText;

        forcePlainTextToggle.checked = false;
    }

    function openMap() {
        console.log("Location: %1".arg(address.text));
        root.locationActivated();
    }

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

            Label {
                Layout.fillWidth: true
                Layout.fillHeight: true
                id: header
                textFormat: Text.AutoText
                text: root.title
                font.bold: true
                wrapMode: Text.Wrap
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
                Label {
                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true
                    text: root.category
                    wrapMode: Text.WordWrap
                }
            }
            RowLayout {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                height: Layout.preferredHeight
                spacing: 6
                Label {
                    text: qsTr("at:")
                    font.bold: true
                }
                Label {
                    Layout.fillWidth: true

                    text: root.locationName
                    wrapMode: Text.Wrap
                    focus: false
                    font.underline: true
                }
            }
            Label {
                Layout.fillWidth: true
                id: description
                wrapMode: Text.Wrap
                textFormat: (forcePlainTextToggle.visible &&
                             forcePlainTextToggle.checked) ? Text.PlainText :
                                                             Text.RichText
                text: (forcePlainTextToggle.visible &&
                       forcePlainTextToggle.checked) ? root.plainTextDescription :
                                                       root.description
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

            CheckBox {
                id: forcePlainTextToggle

                height: Layout.preferredHeight
                Layout.fillWidth: true
                text: qsTr("Force Plaintext")
                visible: root.hasRichText
            }

            RowLayout {
                Layout.fillWidth: true
                height: Layout.preferredHeight
                spacing: 6
                Label {
                    text: qsTr("When:")
                    font.bold: true
                }
                Label {
                    Layout.fillWidth: true
                    text: root.dateTime
                    wrapMode: TextEdit.Wrap
                    font.underline: true
                }
            }
            RowLayout {
                id: durationLayout
                visible: root.duration !== ""
                Layout.fillWidth: true
                height: Layout.preferredHeight
                spacing: 6
                Label {
                    text: qsTr("Duration:")
                    font.bold: true
                }
                Label {
                    Layout.fillWidth: true
                    text: root.duration
                    wrapMode: TextEdit.Wrap
                }
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 6
                visible: root.price !== ""
                Label {
                    text: qsTr("Price:")
                    font.bold: true
                }
                Label {
                    text: root.price
                    Layout.fillWidth: true
                    wrapMode: TextEdit.Wrap
                }
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 6

                Label {
                    text: qsTr("Address:")
                    font.bold: true
                }
                Label {
                    id: address
                    Layout.fillWidth: true
                    wrapMode: TextEdit.Wrap
                    text: root.locationAddress
                    font.underline: true
                    MouseArea {
                        id: addressArea
                        anchors.fill: parent
                        onClicked: root.openMap()
                        cursorShape: Qt.PointingHandCursor
                    }
                    focus: false
                }
                ToolButton {
                    font.family: "Material Design Icons"
                    font.pointSize: (fontPointSize || 16) * 1.5

                    text: MdiFont.Icon.mapSearch
                    onClicked: root.openMap()

                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("View on Map")
                }
            }

            RowLayout {
                Layout.fillWidth: true
                visible: root.directions.length > 0
                spacing: 6
                Label {
                    text: qsTr("Directions:")
                    font.bold: true
                }

                Label {
                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true
                    text: root.directions
                    wrapMode: TextEdit.Wrap
                    focus: false
                }
            }

            Frame {
                Layout.alignment: Qt.AlignHCenter

                background: Item {}

                Row {
                    anchors.fill: parent
                    spacing: 20
                    Column {
                        spacing: 4

                        RoundButton {
                            id: addToCalendar

                            font.family: "Material Design Icons"
                            font.pixelSize: 24
                            text: MdiFont.Icon.calendarPlus

                            onClicked: root.addToCalendarClicked()
                            Material.elevation: 1

                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Add to calendar")
                        }
                        Text {
                            font.pointSize: root.subscriptFontSize
                            text: qsTr("Plan")
                            horizontalAlignment: Text.AlignHCenter
                            anchors.horizontalCenter: addToCalendar.horizontalCenter
                        }
                    }
                    Column {
                        spacing: 4

                        RoundButton {
                            id: show
                            visible: root.urlProvided

                            font.family: "Material Design Icons"
                            font.pixelSize: 24
                            text: MdiFont.Icon.web

                            onClicked: root.openUrlRequested()
                            Material.elevation: 1

                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Show in Browser")
                        }
                        Text {
                            font.pointSize: root.subscriptFontSize
                            text: qsTr("View")
                            horizontalAlignment: Text.AlignHCenter
                            anchors.horizontalCenter: show.horizontalCenter
                        }
                    }
                    Column {
                        spacing: 4

                        RoundButton {
                            id: share
                            visible: root.urlProvided
                            font.family: "Material Design Icons"
                            font.pixelSize: 24
                            text: MdiFont.Icon.shareVariant

                            onClicked: root.shareRequested()
                            Material.elevation: 1
                            Material.accent: Material.Grey

                            ToolTip.visible: hovered
                            ToolTip.text: qsTr("Share...")
                        }
                        Text {
                            font.pointSize: root.subscriptFontSize
                            text: qsTr("Share")
                            horizontalAlignment: Text.AlignHCenter
                            anchors.horizontalCenter: share.horizontalCenter
                        }
                    }
                }
            }
        }
    }
    RoundButton {
        id: closeButton

        visible: !root.currentOSIsAndroid

        z: root.z + 1

        anchors.right:  parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 12
        anchors.rightMargin:  12

        font.family: "Material Design Icons"
        text: MdiFont.Icon.close

        ToolTip.visible: hovered
        ToolTip.text: qsTr("Close")

        onClicked: root.closeClicked()
    }
}
