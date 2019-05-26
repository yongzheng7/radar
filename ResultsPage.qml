import QtQuick 2.12
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.12

import org.radar.app 1.0

FocusScope {
    id: root

    signal itemClicked(int index);

    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
    }

    Component {
        id: sectionHeading
        Rectangle {
            width: root.width
            height: sectionText.font.pixelSize*1.5
            color: palette.midlight

            Text {
                id: sectionText
                anchors.centerIn: parent
                text: section
                color: palette.buttonText
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
                readonly property color textColor: highlighted ? palette.highlightedText : palette.text
                readonly property color standardBgColor: index % 2 === 0 ? palette.window : palette.alternateBase
                readonly property color bgColor: highlighted ? palette.highlight : standardBgColor

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
