// SPDX-FileCopyrightText: 2015 by Aleix Pol <aleixpol@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import org.kde.analitza

Graph3DView {
    id: view
    MouseArea {
        id: mouse
        anchors.fill: parent
        property int lastX: 0
        property int lastY: 0
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

        onPressed: { lastX=mouse.x; lastY=mouse.y }

        onPositionChanged: {
            view.rotate(mouse.x-lastX, mouse.y-lastY)

            lastX=mouse.x
            lastY=mouse.y
        }

        onWheel: {
            view.scale(wheel.angleDelta.y>0 ? 0.9 : 1.1, wheel.x, wheel.y)
        }
    }
}
