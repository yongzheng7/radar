import QtQuick 2.12
import QtLocation 5.12
import QtPositioning 5.12
import QtQuick.Controls 2.12

Rectangle {
    id: root

    property real latitude:  59.91
    property real longitude: 10.75

    signal closeRequested()

    Shortcut {
        sequences: ["Esc", "Back"]
        onActivated: {
            console.log("Shortcut activated");
            root.closeRequested();
        }
        enabled: root.visible
    }

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
        id: mapView

        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(root.latitude, root.longitude)
        zoomLevel: maximumZoomLevel - 1
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

    MapQuickItem {
        id: mapItem
        anchorPoint.x: sourceItem.width /2
        anchorPoint.y: sourceItem.height

        coordinate: QtPositioning.coordinate(root.latitude, root.longitude)

        sourceItem: Image {
            id: image
            source: "qrc:/icons/map-marker-icon.png"
        }
        Component.onCompleted: {
            console.log("Image: %1x%2".arg(sourceItem.width).arg(sourceItem.height));
        }
    }
    Component.onCompleted: {
        mapView.addMapItem(mapItem);
    }
}

