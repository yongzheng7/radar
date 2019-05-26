import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12

import Qt.labs.calendar 1.0
import Qt.labs.platform 1.0

Item {
    id: root

    anchors.fill: parent
    anchors.margins: 12
    Column {
        width: parent.width

        RadioButton {
            id: today
            text: qsTr('Today')
            checked: true
        }
        RadioButton {
            id: thisWeek
            text: qsTr('This Week')
        }
        RadioButton {
            id: thisMonth
            text: qsTr('This Month')
        }
        RadioButton {
            id: customRange
            text: qsTr('Custom day')
        }

        ColumnLayout {
            DayOfWeekRow {
                Layout.fillWidth: true
            }

            MonthGrid {
                id: grid
                Layout.fillWidth: true
            }
        }
    }
}
