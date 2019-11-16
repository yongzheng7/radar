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
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Universal 2.12
import Qt.labs.settings 1.0

import Qt.labs.platform 1.1 as Platform

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
    title: qsTr("Radar App")

    readonly property bool currentOSIsAndroid: Qt.platform.os === "android"

    Shortcut {
        id: backShortcut
        sequences: root.currentOSIsAndroid ? ["Esc", "Back"] : ["Esc"]
        onActivated: root.setPrevious()
        enabled: swipeView.currentIndex > 0 && !eventPage.active && !mapView.active
    }

    header: ToolBar {
        Material.foreground: "white"
        Material.background: "tomato"

        RowLayout {
            spacing: 20
            anchors.fill: parent

            ToolButton {
                icon.name: swipeView.currentIndex > 0 ? "back" : ""
                enabled: swipeView.currentIndex > 0
                //text: icon.name === "" ? "☰" : ""
                onClicked: {
                    if (mapView.active) {
                        mapView.closeView();
                        return;
                    }

                    if (eventPage.active) {
                        eventPage.closePage();
                        return;
                    }

                    if (swipeView.currentIndex > 0) {
                        root.setPrevious();
                    } else {
                        //drawer.open()
                    }
                }
            }

            Label {
                id: titleLabel
                text: {
                    if (mapView.active) {
                        return qsTr("Event on Map");
                    }
                    if (eventPage.active) {
                        return qsTr("Event");
                    }
                    switch (swipeView.currentItem) {
                    case location:
                        return qsTr("Location");
                    case results:
                        return qsTr("Ongoing events");
                    default:
                        return qsTr("Radar")
                    }
                }
                font.pixelSize: 20
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                text: "⋮"
                font.pointSize: root.font.pointSize*1.5
                font.bold: true
                Layout.alignment: Qt.AlignVCenter

                onClicked: optionsMenu.open()

                Menu {
                    id: optionsMenu
                    x: parent.width - width
                    transformOrigin: Menu.TopRight

//                    MenuItem {
//                        text: "Settings"
//                        //onTriggered: settingsDialog.open()
//                    }
                    MenuItem {
                        text: qsTr("About")
                        onTriggered: aboutDialog.open()
                    }
                    MenuItem {
                        text: qsTr("Share App...")
                        onTriggered: App.shareApp()
                    }
                    MenuItem {
                        text: qsTr("Share via QR-code...")
                        onTriggered: qrCodeDialog.open()
                    }
                }
            }
        }
    }

    Dialog {
        id: aboutDialog
        anchors.centerIn: parent
        horizontalPadding: 20
        margins: 20

        title: qsTr("Radar App")

        Label {
            anchors.fill: parent
            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            text: qsTr("<p>Copyright © 2019</p>" +
                       "<p>This program comes with ABSOLUTELY NO WARRANTY.</p>" +
                       "<p>This is free software, and you are welcome to redistribute it under certain conditions.</p>"+
                       "<p><a href=\"https://www.gnu.org/licenses/gpl-3.0.en.html\">Details on License…</a></p>" +
                       "<p><a href=\"https://0xacab.org/xandyx/radar-app/tree/master\">Source code…</a></p>")
            onLinkActivated: App.openLink(link)
        }
    }

    Dialog {
        id: qrCodeDialog
        anchors.centerIn: parent
        width: Math.min(parent.height*3/4, parent.width*3/4) - 40
        height: width + 40

        title: qsTr("Radar App")

        Image {
            id: image
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: "images/qrcode-apk.png"
        }
    }

    Platform.MessageDialog {
        id: noMapApplication

        text: qsTr("No Maps application available.")
        informativeText: qsTr("Do you want to see event location with web browser?")

        buttons: Platform.MessageDialog.No | Platform.MessageDialog.Yes

        onYesClicked: Qt.openUrlExternally(App.url)
    }

    Platform.MessageDialog {
        id: loadFailedDialog

        text: qsTr("Failed to load data")
        informativeText: qsTr("Network Errror")

        buttons: Platform.MessageDialog.Ok
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
                readonly property int index: SwipeView.index
            }

//            Loader {
//                id: dateTime
//                active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem
//                source: "qrc:/DateTimePage.qml"
//            }

            Loader {
                id: results
                active: App.isLoaded && (SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem)
                source: "qrc:/ResultsPage.qml"
                readonly property int index: SwipeView.index
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
                    if (index === location.index && !App.isLoaded) {
                        return false;
                    }
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

        function closePage() {
            eventPage.active = false;
            swipeView.enabled = true;
        }
        Connections {
            target: eventPage.item
            onCloseClicked: eventPage.closePage()
            onLinkActivated: App.openLink(link)
            onLocationActivated: {
                if (root.currentOSIsAndroid) {
                    App.showLocation();
                } else {
                    mapView.active = true;
                    eventPage.item.enabled = false;
                    if (mapView.item) {
                        mapView.updateCoordinates();
                    }
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
            onFailedToOpenMapApp: noMapApplication.open()
        }
    }

    Popup {
        id: pleaseWait

        parent: Overlay.overlay
        anchors.centerIn: parent

        horizontalPadding: 20
        verticalPadding: 20

        z: 100

        focus: true
        modal: true
        contentItem: Row {
            spacing: 6
            BusyIndicator {
                id: indicator
                anchors.verticalCenter: parent.verticalCenter
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 6
                Text {
                    text: qsTr("Please wait…")
                }
                Text {
                    visible: text !== ""
                    text: {
                        switch (appState) {
                        case AppStates.Loading:
                            return qsTr("Getting events");
                        case AppStates.Filtering:
                            return qsTr("Filtering events");
                        case AppStates.CountryLoad:
                            return qsTr("Loading coutries");
                        case AppStates.CitiesLoad:
                            return qsTr("Loading cities");
                        default:
                            return "";
                        }
                    }
                }
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

            onCloseRequested: mapView.closeView()
        }

        function closeView() {
            console.log("closeView()");
            mapView.active = false;
            eventPage.active = true;
            eventPage.item.enabled = true;
        }

        source: "qrc:/map.qml"
        onLoaded: updateCoordinates()
        function updateCoordinates() {
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
        onLoadFailed: loadFailedDialog.open()
    }
}
