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
        enabled: swipeView.currentIndex > 0 && !eventPage.active
    }

    ColumnLayout {
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

            Loader {
                id: dateTime
                active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem
                source: "qrc:/DateTimePage.qml"
            }

            Loader {
                id: results
                active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem
                source: "qrc:/ResultsPage.qml"
                Connections {
                    target: results.item
                    onItemClicked: {
                        console.log("Setting event details for item %1".arg(index));
                        App.selectEvent(index);
                        eventPage.active = true;
                    }
                }
            }
        }

        PageIndicator {
            id: swipeIndicator

            Layout.alignment: Qt.AlignCenter

            count: swipeView.count
            currentIndex: swipeView.currentIndex
        }

        RowLayout {
            id: buttons
            Layout.preferredWidth: swipeView.width
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: 0
            Layout.bottomMargin: 12
            Layout.leftMargin: 12
            Layout.rightMargin: 12

            Button {
                id: back
                Layout.alignment: Qt.AlignLeft
                text: qsTr("< Back")
                enabled: swipeView.currentIndex > 0
                onClicked: root.setPrevious()
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
            }
            onLinkActivated: App.openLink(link)
        }
        onStatusChanged: {
            if (eventPage.status === Loader.Ready) {
                eventPage.item.title = App.title;
                eventPage.item.description = App.description;
                eventPage.item.category = App.category;
                eventPage.item.dateTime = App.dateTime;
                eventPage.item.price = App.price;
                eventPage.item.locationName = App.locationName;
                eventPage.item.locationAddress = App.locationAddress;
                eventPage.item.city = App.eventCity;
                eventPage.item.country = App.eventCountry;
            }
        }
        Connections {
            target: App
            onCurrentEventChanged: {
                if (eventPage.item) {
                    console.log("Updating location details...");
                    eventPage.item.city = App.eventCity;
                    eventPage.item.country = App.eventCountry;
                    eventPage.item.locationName = App.locationName;
                    eventPage.item.locationAddress = App.locationAddress;
                    eventPage.item.directions = App.directions;
                }
            }
            onCurrentLocationChanged: {
                eventPage.item.locationName = App.locationName || "";
                eventPage.item.locationAddress = App.locationAddress || "";
                eventPage.item.directions = App.directions || "";
            }
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
                 appState === AppStates.Filtering
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
