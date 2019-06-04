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

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 6

        Button {
            id: closeButton
            Layout.alignment: Qt.AlignTop | Qt.AlignTrailing
            Layout.margins: 4
            text: qsTr("Close")
            onClicked: root.closeClicked()
        }

        Keys.onBackPressed: {
            root.closeClicked();
        }

        Keys.onEscapePressed: root.closeClicked()

        Flickable {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: parent.width
            contentWidth: parent.width
            contentHeight: column.implicitHeight
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
                    spacing: 6
                    Label {
                        text: qsTr("at:")
                        font.bold: true
                    }
                    TextEdit {
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
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                console.log("Location: %1".arg(address.text));
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

                Label {
                    Layout.fillWidth: true
                    text: root.city
                }

                Label {
                    Layout.fillWidth: true
                    text: root.country
                }
            }
        }
    }
}
