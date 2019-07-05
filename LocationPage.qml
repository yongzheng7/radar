import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12

import org.radar.app 1.0

Item {
    id: root
    anchors.fill: parent
    anchors.margins: 12
    Column {
        anchors.fill: parent
        Text {
            id: countryLabel
            text: qsTr("Country")
        }
        ComboBox {
            id: countries
            width: parent.width
            model: App.countries

            onActivated: {
                App.country = model[index];
            }
            onModelChanged: {
                countries.currentIndex = find(App.country);
            }
        }

        Text {
            id: cityLabel
            text: qsTr("City")
        }
        ComboBox {
            id: cities
            width: parent.width
            //textRole: "name"
            model: App.cities

            onActivated: {
                App.city = model[index];
            }
        }
        CheckBox {
            id: remember
            checked: false
            text: qsTr("Remember location")
        }
    }

    Connections {
        target: App
        onCityChanged: {
//            selectCity(App.city);
        }
    }
//    function selectCity(city) {
//        if (cities.currentIndex !== -1 && cities.model.get(cities.currentIndex).city === city) {
//            return;
//        }

//        console.log("selectCity %1".arg(city));
//        var idx;
//        for (idx = 0; idx < cities.count; ++idx) {
//            if (cities.model.get(idx).city === city) {
//                console.log("found!");
//                cities.currentIndex = idx;
//                return;
//            }
//        }
//    }
    Component.onCompleted: {
        //selectCity(App.city);
    }
}
