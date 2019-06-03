import QtQuick 2.12
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import org.radar.app 1.0

FocusScope {
    id: root

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
                readonly property bool highlighted: mouseArea.containsMouse
                readonly property color textColor: Material.color(Material.Grey, highlighted ? Material.Shade50 : Material.Shade900)
                readonly property color standardBgColor: Material.color(Material.Grey, index % 2 === 0 ? Material.Shade100 : Material.Shade50)
                readonly property color bgColor: highlighted ? Material.color(Material.LightBlue, Material.Shade800) : standardBgColor

                height: row.implicitHeight
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

                RowLayout {
                    id: row
                    width: root.width
                    spacing: 6
                    Text {
                        text: model.startDateTime
                        font.italic: true
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignTop
                        Layout.alignment: Qt.AlignLeading | Qt.AlignTop
                        Layout.margins: 4
                        color: mouseArea.textColor
                    }
                    Text {
                        text: model.title
                        font.bold: true
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignRight
                        Layout.alignment: Qt.AlignTrailing
                        Layout.fillWidth: true
                        Layout.margins: 4
                        color: mouseArea.textColor
                    }
                }
            }
        }

        section.property: "date"
        section.criteria: ViewSection.FullString
        section.delegate: sectionHeading

        ScrollIndicator.vertical: ScrollIndicator {}
    }
}
