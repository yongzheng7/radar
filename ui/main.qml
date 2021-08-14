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
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.11

import org.radar.app 1.0

import "Icon.js" as MdiFont

ApplicationWindow {
    id: root

    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    font.pointSize: fontPointSize || 32
    visible: true
    width: 800
    height: 600
    title: qsTr("Radar App")

    readonly property bool currentOSIsAndroid: Qt.platform.os === "android"

    Shortcut {
        id: backShortcut
        sequences: root.currentOSIsAndroid ? ["Esc", "Back"] : ["Esc"]
        onActivated: root.setPrevious()
        enabled: swipeView.currentIndex > 0 && !eventView.active && !mapView.active
    }

    header: ToolBar {
        RowLayout {
            spacing: 20
            anchors.fill: parent

            ToolButton {
                id: toolBarBackButton
                icon.name: swipeView.currentIndex > 0 ? "back" : ""
                enabled: swipeView.currentIndex > 0
                onClicked: {
                    if (mapView.active) {
                        mapView.closeView();
                        return;
                    }

                    if (eventView.active) {
                        eventView.closePage();
                        return;
                    }

                    if (swipeView.currentIndex > 0) {
                        root.setPrevious();
                        return;
                    }
                }
            }

            Label {
                id: titleLabel
                text: {
                    if (mapView.active) {
                        return qsTr("Event on Map");
                    }
                    if (eventView.active) {
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

                font.pixelSize: toolBarBackButton.icon.height
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }


            ToolButton {
                visible: !eventView.active
                icon.name: "menu"
                font.bold: true
                Layout.alignment: Qt.AlignVCenter

                onClicked: optionsMenu.open()
            }

            ToolButton {
                visible: eventView.active || mapView.active

                font.family: "Material Design Icons"
                font.bold: true
                font.pixelSize: toolBarBackButton.icon.height

                text: MdiFont.Icon.close
                Layout.alignment: Qt.AlignVCenter

                onClicked: {
                    if (mapView.active) {
                        mapView.closeView();
                        return;
                    }
                    eventView.closePage();
                }
            }
        }
    }

    Menu {
        id: optionsMenu

        width: Math.max(reloadCountriesItem.implicitWidth,
                        aboutItem.implicitWidth,
                        shareItem.implicitWidth,
                        shareWithQrItem.implicitWidth)

        x: parent.width - width
        y: 0

        MenuItem {
            id: reloadCountriesItem
            text: qsTr("Reload Countries")
            onTriggered: App.refreshCountries();
        }

        MenuItem {
            id: aboutItem
            text: qsTr("About")
            onTriggered: aboutDialog.open()
        }
        MenuItem {
            id: shareItem
            text: qsTr("Share App...")
            onTriggered: App.shareApp()
        }
        MenuItem {
            id: shareWithQrItem
            text: qsTr("Share via QR-code...")
            onTriggered: qrCodeDialog.open()
        }
    }

    Dialog {
        id: aboutDialog

        parent: Overlay.overlay
        anchors.centerIn: parent

        modal: true

        header: Label {
            text: qsTr("Radar App")
            wrapMode: Text.NoWrap
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pointSize: root.font.pointSize
        }

        Label {
            anchors.fill: parent
            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            text: qsTr("<p>Copyright © 2019-2020</p>" +
                       "<p>This program comes with ABSOLUTELY NO WARRANTY.</p>" +
                       "<p>This is free software, and you are welcome to redistribute it under certain conditions.</p>"+
                       "<p><a href=\"https://www.gnu.org/licenses/gpl-3.0.en.html\">Details on License…</a></p>" +
                       "<p><a href=\"https://0xacab.org/xandyx/radar-app/tree/master\">Source code…</a></p>")
            onLinkActivated: App.openLink(link)
        }
    }

    Dialog {
        id: qrCodeDialog

        parent: Overlay.overlay
        anchors.centerIn: parent

        width: image.width + 2*padding
        height: image.height + 2*padding + implicitHeaderHeight + implicitFooterHeight

        modal: true

        header: Label {
            text: qsTr("Scan QR Code to download")
            wrapMode: Text.WordWrap
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
        }

        Frame {
            width: Math.min(root.width-60, root.height-60, image.sourceSize.width) - qrCodeDialog.padding
            height: width

            anchors.centerIn: parent
            anchors.margins: 12

            Image {
                id: image

                anchors.fill: parent
                anchors.margins: 0

                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/qrcode-apk.png"
            }
        }
    }

    Dialog {
        id: noMapApplication

        modal: true

        parent: Overlay.overlay
        anchors.centerIn: parent

        title: qsTr("No Maps application available.")

        Label {
            wrapMode: Text.WordWrap
            text: qsTr("Do you want to see event location with web browser?")
        }

        standardButtons: Dialog.No | Dialog.Yes

        onAccepted: Qt.openUrlExternally(App.url)
    }

    Dialog {
        id: loadFailedDialog

        modal: true

        parent: Overlay.overlay
        anchors.centerIn: parent

        title: qsTr("Failed to load data")
        Label {text: qsTr("Network Errror")}

        standardButtons: Dialog.Ok
        onAccepted: loadFailedDialog.close()
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
                source: "qrc:/ui/LocationPage.qml"
                readonly property int index: SwipeView.index

                Connections {
                    target: location.item
                    onShowClicked: root.setNext()
                }
            }

            Loader {
                id: results
                active: App.isLoaded && (SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem)
                source: "qrc:/ui/ResultsPage.qml"
                readonly property int index: SwipeView.index
                property bool initialPositioningNeeded: false

                Connections {
                    target: results.item
                    onItemClicked: {
                        console.log("Setting event details for item %1".arg(index));
                        App.selectEvent(index);
                        swipeView.enabled = false;
                        eventView.active = true;
                    }
                }
                onLoaded: {
                    if (initialPositioningNeeded) {
                        initialPositioningNeeded = false;
                        item.positionToTodaysEvents();
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

                visible: !root.currentOSIsAndroid
                enabled: swipeView.currentIndex > 0

                Layout.alignment: Qt.AlignLeft

                text: qsTr("< Back")

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

                visible: !root.currentOSIsAndroid
                enabled: {
                    var index = swipeView.currentIndex;
                    if (index === location.index && !App.isLoaded) {
                        return false;
                    }
                    return index >= 0 && index < swipeView.count - 1;
                }
                focus: !root.currentOSIsAndroid

                Layout.alignment: Qt.AlignRight

                text: qsTr("Next >")

                onClicked: root.setNext()
            }
        }
    }

    Loader {
        anchors.fill: parent
        z: 1
        id: eventView
        active: false
        source: "qrc:/ui/EventView.qml"

        function closePage() {
            eventView.active = false;
            swipeView.enabled = true;
        }
        Connections {
            target: eventView.item
            onCloseClicked: eventView.closePage()
            onLinkActivated: App.openLink(link)
            onLocationActivated: {
                if (root.currentOSIsAndroid) {
                    App.showLocation();
                } else {
                    mapView.active = true;
                    eventView.item.enabled = false;
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
            if (eventView.item) {
                console.log("Updating event details...");
                eventView.item.updateEventInfo();
            }
        }
        function updateLocationInfo() {
            if (eventView.item) {
                console.log("Updating location details...");
                eventView.item.updateLocationInfo();
            }
        }
        onLoaded: {
            updateEventInfo();
            updateLocationInfo();
        }

        Connections {
            target: App
            onCurrentEventChanged: eventView.updateEventInfo()
            onCurrentLocationChanged: eventView.updateLocationInfo()
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

        contentItem: GridLayout {
            columnSpacing: 10
            rowSpacing: 10
            rows: 2
            columns: 2

            anchors.fill: parent

            BusyIndicator {
                Layout.column: 0
                Layout.row: 0
                Layout.rowSpan: 2
                Layout.margins: 10
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                id: indicator
            }

            Label {
                Layout.column: 1
                Layout.row: 0
                text: qsTr("Please wait…")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
            }
            Label {
                id: popupMessage
                Layout.column: 1
                Layout.row: 1
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.preferredWidth: popupMessage.implicitWidth

                wrapMode: Text.WordWrap

                visible: text !== ""
                text: ""
            }
        }

        visible:  ((swipeView.currentIndex === location.index) && (appState === AppStates.Loading ||
                                                                   appState === AppStates.Filtering ||
                                                                   appState === AppStates.Extraction)) ||
                  appState === AppStates.CountryLoad ||
                  appState === AppStates.CountryFilter ||
                  appState === AppStates.CitiesLoad


        function updatePopupText() {
            switch (appState) {
            case AppStates.Loading:
                popupMessage.text = qsTr("Getting events");
                break;
            case AppStates.Filtering:
                popupMessage.text = qsTr("Filtering events");
                break;
            case AppStates.CountryLoad:
                popupMessage.text = qsTr("Loading countries");
                break;
            case AppStates.CitiesLoad:
                popupMessage.text = qsTr("Loading cities");
                break;
            default:
                break;
            }
        }

        onVisibleChanged: pleaseWait.updatePopupText()
        Connections {
            target: root
            onAppStateChanged: pleaseWait.updatePopupText()
        }

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
            eventView.active = true;
            eventView.item.enabled = true;
        }

        source: root.currentOSIsAndroid ? "" : "qrc:/ui/map.qml"
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
        console.log("currentOS is android=" + root.currentOSIsAndroid);
    }

    readonly property var appState: App.state

    Connections {
        target: App

        onStateChanged: {
            console.log("State=%1".arg(App.state));
            if (App.state === AppStates.Error) {
                loadFailedDialog.open();
            }
        }

        onIsFirstLoadChanged: {
            if (App.isFirstLoad) {
                results.initialPositioningNeeded = true;
            }
        }

        onIsLoadedChanged: {
            console.log("App.isLoaded=%1".arg(App.isLoaded));
            if (App.isLoaded) {
                if (results.active && results.item && results.initialPositioningNeeded) {
                    results.item.positionToTodaysEvents();
                    results.initialPositioningNeeded = false;
                }
            }
        }
    }
}
