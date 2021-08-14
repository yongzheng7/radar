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
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import org.radar.app 1.0

FocusScope {
    id: root

    clip: true

    readonly property bool currentOSIsAndroid: Qt.platform.os === "android"

    signal itemClicked(int index);

    function positionToTodaysEvents() {
        console.log("positionToTodaysEvents()");
        var indexOfTodaysFirstEvent = App.getFirstTodaysItemIndex();
        if (indexOfTodaysFirstEvent !== -1) {
            resultsList.positionViewAtIndex(indexOfTodaysFirstEvent, ListView.Beginning);
        }
    }

    BusyIndicator {
        id: busyIndicator

        running: appState === AppStates.Loading ||
                 appState === AppStates.Filtering ||
                 appState === AppStates.Extraction

        anchors.bottom: root.bottom
        anchors.left: root.left
        anchors.right: root.right

        z: 2
    }

    Component {
        id: sectionHeading
        Item {
            y: -1
            width: root.width
            height: sectionText.font.pixelSize*2.0 //1.5

            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 0.5
                color: Material.color(Material.Grey, Material.Shade500)
            }


            Text {
                id: sectionText
                anchors.centerIn: parent
                text: section
                font.capitalization: Font.AllUppercase
                font.pointSize: fontPointSize * 1.2
                color: Material.color(Material.Pink, Material.Shade800)
            }
        }
    }

    Label {
        id: noResults

        visible: App.noEventsFound
        anchors.fill: parent

        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: qsTr("Nothing found for %1 %2".arg(App.country).arg(App.city))

    }

    ListView {
        id: resultsList

        activeFocusOnTab: true

        visible: !App.noEventsFound

        onVisibleChanged: {
            console.log("!! resultsList.visible=%1".arg(resultsList.visible));
        }

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: busyIndicator.running ? busyIndicator.top : parent.bottom
        Behavior on anchors.bottom {
            PropertyAnimation { duration: 1000 }
        }

        model: App.eventsModel
        highlightFollowsCurrentItem: false
        interactive: true
        keyNavigationEnabled: true

        clip: false

        boundsMovement: Flickable.FollowBoundsBehavior
        boundsBehavior: Flickable.DragOverBounds

        opacity: Math.max(0.0, 1.0 - Math.abs(verticalOvershoot) / height)

        property var overShoot: resultsList.verticalOvershoot

        onOverShootChanged: {
            console.log("% verticalOvershoot=" + (verticalOvershoot/height) * 100);
            if (verticalOvershoot > root.height/5) {
                console.log("Load more needed...");
                App.reloadEvents();
            }
        }

        delegate: Loader {
            width: root.width
            sourceComponent: MouseArea {
                id: mouseArea
                readonly property bool highlighted: resultsList.currentIndex === index ||
                                                    (!root.currentOSIsAndroid && mouseArea.containsMouse)
                readonly property color textColor: Material.color(Material.Grey, highlighted ? Material.Shade50 : Material.Shade900)
                readonly property color standardBgColor: Material.color(Material.Grey, index % 2 === 0 ? Material.Shade100 : Material.Shade50)
                readonly property color bgColor: highlighted ? Material.color(Material.LightBlue, Material.Shade800) : standardBgColor

                height: title.contentHeight +
                        2 * title.anchors.margins +
                        Math.max((location.visible ? (location.contentHeight + location.anchors.margins) : 0),
                                 (distance.visible ? (distance.contentHeight + distance.anchors.margins) : 0))

                hoverEnabled: true
                onClicked: {
                    console.log("index: " + index);
                    resultsList.currentIndex = index;
                    mouseArea.forceActiveFocus();
                    root.itemClicked(index);
                }

                Rectangle {
                    anchors.fill: parent
                    color: mouseArea.bgColor
                }

                Text {
                    id: date

                    anchors.left: mouseArea.left
                    anchors.top: mouseArea.top
                    anchors.topMargin: 4
                    anchors.leftMargin: 4

                    text: model.startDateTime
                    textFormat: Text.PlainText
                    font.italic: true
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignTop
                    color: mouseArea.textColor
                }
                Text {
                    id: title

                    anchors.top: mouseArea.top
                    anchors.right: mouseArea.right
                    anchors.margins: 4
                    anchors.bottom: location.visible ? location.top : mouseArea.bottom
                    anchors.left: date.right

                    text: {
                        var category = model.category;
                        return category === "" ?
                                    model.title :
                                    "<strong>%1</strong> [%2]".arg(model.title).arg(category);
                    }
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignRight
                    color: mouseArea.textColor
                }
                Text {
                    id: distance
                    text: model.distance
                    textFormat: Text.PlainText
                    anchors.margins: 4
                    anchors.top: date.bottom
                    anchors.left: mouseArea.left
                    anchors.bottom: mouseArea.bottom
                    verticalAlignment: Text.AlignBottom
                    color: mouseArea.textColor
                }
                Text {
                    id: location

                    visible: location.text !== ""

                    anchors.margins: 4
                    anchors.bottom: mouseArea.bottom
                    anchors.right: mouseArea.right
                    anchors.left: distance.text !== "" ? distance.right : date.right

                    text: model.locationName
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignRight
                    color: mouseArea.textColor
                }
            }
        }

        section.property: "date"
        section.criteria: ViewSection.FullString
        section.delegate: sectionHeading

        ScrollIndicator.vertical: ScrollIndicator {}
    }
}
