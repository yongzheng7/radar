/*
 *   Copyright (c) 2019-2021 <xandyx_at_riseup dot net>
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
