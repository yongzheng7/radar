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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.11

import org.radar.app 1.0

ApplicationWindow {
    id: root

    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    font.pointSize: fontPointSize || 18
    visible: true
    width: 800
    height: 600
    title: qsTr("Radar")

    readonly property bool currentOSIsAndroid: Qt.platform.os === "android"

    Shortcut {
        sequences: root.currentOSIsAndroid ? ["Esc", "Back"] : ["Esc"]
        onActivated: root.setPrevious()
        enabled: swipeView.currentIndex > 0 && !eventPage.active && !mapView.active
    }

    ColumnLayout {
        id: column
        anchors.fill: parent
        spacing: 6
        anchors.margins: 0

        SwipeView {
            id: swipeView

            orientation: Qt.Horizontal

            Layout.fillWidth: true
            Layout.fillHeight: true
            interactive: root.currentOSIsAndroid

            Loader {
                id: location
                active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem
                source: "qrc:/LocationPage.qml"
            }

//            Loader {
//                id: dateTime
//                active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem
//                source: "qrc:/DateTimePage.qml"
//            }

            Loader {
                id: results
                active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem
                source: "qrc:/ResultsPage.qml"
                Connections {
                    target: results.item
                    onItemClicked: {
                        console.log("Setting event details for item %1".arg(index));
                        App.selectEvent(index);
                        swipeView.enabled = false;
                        eventPage.active = true;
                    }
                }
            }

            onCurrentItemChanged: {
                if (currentItem === results) {
                    App.startUpdatePosition();
                } else {
                    App.stopUpdatePosition();
                }
            }
        }

        RowLayout {
            id: buttons

            Layout.preferredWidth: swipeView.width
            Layout.topMargin: 0
            Layout.bottomMargin: 12
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            Layout.fillWidth: true
            Layout.fillHeight: true

            Button {
                id: back
                Layout.alignment: Qt.AlignLeft
                text: qsTr("< Back")
                enabled: swipeView.currentIndex > 0
                onClicked: root.setPrevious()
            }
            PageIndicator {
                id: swipeIndicator
                Layout.alignment: Qt.AlignCenter

                count: swipeView.count
                currentIndex: swipeView.currentIndex
            }
            Button {
                id: forward
                Layout.alignment: Qt.AlignRight
                text: qsTr("Next >")
                focus: true
                enabled: {
                    var index = swipeView.currentIndex;
                    return index >= 0 && index < swipeView.count - 1;
                }
                onClicked: root.setNext()
            }
        }
    }

    Loader {
        anchors.fill: parent
        z: 1
        id: eventPage
        active: false
        source: "qrc:/EventPage.qml"

        Connections {
            target: eventPage.item
            onCloseClicked: {
                eventPage.active = false;
                swipeView.enabled = true;
            }
            onLinkActivated: App.openLink(link)
            onLocationActivated: {
                if (root.currentOSIsAndroid) {
                    App.showLocation();
                } else {
                    mapView.active = true;
                    eventPage.item.enabled = false;
                }
            }
            onAddToCalendarClicked: App.addToCalendar()
            onShareRequested: App.share()
            onOpenUrlRequested: Qt.openUrlExternally(App.url)
        }

        function updateEventInfo() {
            if (eventPage.item) {
                console.log("Updating event details...");
                eventPage.item.title = App.title;
                eventPage.item.description = App.description;
                eventPage.item.category = App.category;
                eventPage.item.dateTime = App.dateTime;
                eventPage.item.duration = App.duration
                eventPage.item.price = App.price;
                eventPage.item.locationName = App.locationName;
                eventPage.item.locationAddress = App.locationAddress;
                eventPage.item.city = App.eventCity;
                eventPage.item.country = App.eventCountry;
                eventPage.item.directions = App.directions;
                eventPage.item.urlProvided = App.url !== ""
            }
        }
        function updateLocationInfo() {
            if (eventPage.item) {
                console.log("Updating location details...");
                eventPage.item.locationName = App.locationName;
                eventPage.item.locationAddress = App.locationAddress;
                eventPage.item.directions = App.directions;
            }
        }
        onLoaded: {
            updateEventInfo();
            updateLocationInfo();
        }

        Connections {
            target: App
            onCurrentEventChanged: eventPage.updateEventInfo()
            onCurrentLocationChanged: eventPage.updateLocationInfo()
        }
    }

    Popup {
        id: pleaseWait

        parent: Overlay.overlay
        z: 100
        x: Math.round((parent.width - width) / 2)
        y: Math.round((parent.height - height)/2)
        width: contentItem.implicitWidth + 20
        height: contentItem.implicitHeight + 20

        focus: true
        modal: true
        contentItem: Row {
            spacing: 6
            BusyIndicator {
                id: indicator
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Loading...")
            }
        }

        visible: appState === AppStates.Loading ||
                 appState === AppStates.Filtering ||
                 appState === AppStates.CountryLoad ||
                 appState === AppStates.CountryFilter ||
                 appState === AppStates.CitiesLoad ||
                 appState === AppStates.Extraction

        onClosed: App.cancelOperation()
    }

    Loader {
        id: mapView
        active: false
        anchors.fill: parent
        z: 2

        Connections {
            target: mapView.item

            onCloseRequested: {
                console.log("onCloseRequested");
                mapView.active = false;
                eventPage.active = true;
                eventPage.item.enabled = true;
            }
        }

        source: "qrc:/map.qml"
        onLoaded: {
            mapView.item.latitude = App.latitude;
            mapView.item.longitude = App.longitude;
        }
    }

    function setNext() {
        console.log("setNext");
        if (swipeView.currentIndex === swipeView.count - 1) {
            return;
        }
        swipeView.currentIndex += 1;
    }

    function setPrevious() {
        console.log("setPrevious");
        if (swipeView.currentIndex < 1) {
            return;
        }
        swipeView.currentIndex -= 1;
    }

    Component.onCompleted: {
        console.log("App.isLoaded=%1".arg(App.isLoaded));
        swipeView.currentIndex = 0;
        App.reload();
        console.log("currentOS is android=" + root.currentOSIsAndroid)
    }

    readonly property var appState: App.state

    Connections {
        target: App
        onStateChanged: {
            console.log("State=%1".arg(App.state));
        }
    }
}
