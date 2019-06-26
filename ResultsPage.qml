import QtQuick 2.12
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import org.radar.app 1.0

FocusScope {
    id: root

    readonly property bool currentOSIsAndroid: Qt.platform.os === "android"

    signal itemClicked(int index);

    Component {
        id: sectionHeading
        Rectangle {
            width: root.width
            height: sectionText.font.pixelSize*1.5
            color: Material.color(Material.Grey, Material.Shade400)

            Text {
                id: sectionText
                anchors.centerIn: parent
                text: section
                color: Material.color(Material.Grey, Material.Shade800)
                font.bold: true
                font.pointSize: fontPointSize * 1.2
            }
        }
    }

    ListView {
        id: resultsList

        activeFocusOnTab: true

        anchors.fill: parent
        model: App.eventsModel
        highlightFollowsCurrentItem: false
        interactive: true
        keyNavigationEnabled: true

        clip: true

        delegate: Loader {
            width: root.width
            sourceComponent: MouseArea {
                id: mouseArea
                readonly property bool highlighted: mouseArea.containsPress || (!root.currentOSIsAndroid && mouseArea.containsMouse)
                readonly property color textColor: Material.color(Material.Grey, highlighted ? Material.Shade50 : Material.Shade900)
                readonly property color standardBgColor: Material.color(Material.Grey, index % 2 === 0 ? Material.Shade100 : Material.Shade50)
                readonly property color bgColor: highlighted ? Material.color(Material.LightBlue, Material.Shade800) : standardBgColor

                height: title.contentHeight +
                        2 * title.anchors.margins +
                        (location.visible ? (location.contentHeight + location.anchors.margins) : 0)

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

                    text: model.title
                    font.bold: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignRight
                    color: mouseArea.textColor
                }
                Text {
                    id: location

                    visible: location.text !== ""

                    anchors.margins: 4
                    anchors.bottom: mouseArea.bottom
                    anchors.right: mouseArea.right

                    text: model.locationName
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
