import QtQuick 2.12
import QtLocation 5.12
import QtPositioning 5.12
import QtQuick.Controls 2.12

Rectangle {
    id: root

    anchors.fill: parent

    property var latitude:  59.91
    property var longitude: 10.75

    signal closeRequested()

    Shortcut {
        sequences: ["Esc", "Back"]
        onActivated: {
            console.log("Shortcut activated");
            root.closeRequested();
        }
        enabled: root.visible
    }

    focus: true

    Keys.onBackPressed: root.closeRequested()
    Keys.onEscapePressed: root.closeRequested()

    Plugin {
        id: mapPlugin
        name: "osm" // "mapboxgl", "esri", ...
        // specify plugin parameters if necessary
        // PluginParameter {
        //     name:
        //     value:
        // }
    }

    Map {
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(root.latitude, root.longitude)
        zoomLevel: 14
    }

    Button {
        text: qsTr("Close");
        z: root.z + 1
        anchors.right:  parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 12
        anchors.rightMargin:  12

        onClicked: {
            console.log("Close!");
            root.closeRequested();
        }
    }
}

