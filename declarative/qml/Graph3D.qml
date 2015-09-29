/*************************************************************************************
 *  Copyright (C) 2015 by Aleix Pol <aleixpol@kde.org>                               *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.kde.analitza 1.1

Graph3DView {
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
