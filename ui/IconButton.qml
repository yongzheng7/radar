import QtQuick 2.12
import QtQuick.Controls 2.12

Button {
    id: root

    icon.name: "empty"

    property string iconCode: ""
    property alias color: root.palette.buttonText

    contentItem: Row {
        spacing: 6
        Text {
            horizontalAlignment: Text.AlignHCenter
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            verticalAlignment: Text.AlignVCenter
            font.family: "Material Design Icons"
            font.bold: true
            font.pixelSize: root.font.pixelSize
            opacity: root.opacity
            color: root.color
            text: root.iconCode
        }

        Text {
            horizontalAlignment: Text.AlignHCenter
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            verticalAlignment: Text.AlignVCenter

            text: root.text
            font: root.font
            opacity: root.opacity
            color: root.color
            elide: Text.ElideRight
        }
    }
}
