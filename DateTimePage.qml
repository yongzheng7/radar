/*
 *   Copyright (c) 2019 <xandyx_at_riseup dot net>
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
