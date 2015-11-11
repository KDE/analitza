import QtQuick 2.0
import org.kde.analitza 1.0

Graph2DView {
	id: view
	anchors.fill: parent
	
	showGrid: true
	ticksShown: true

	PinchArea {
		anchors.fill: parent
		property double thePreviousScale: 1
		
		function distance(p1, p2)
		{
			var w=p2.x-p1.x
			var h=p2.y-p1.y
			return Math.sqrt(w*w+h*h)
		}
		
		onPinchStarted: thePreviousScale=1
		
		onPinchUpdated: {
			var currentDistance = distance(pinch.point1, pinch.point2)
			if(currentDistance>0) {
				var doScale = thePreviousScale/pinch.scale
				view.scale(doScale, pinch.center.x, pinch.center.y)
				thePreviousScale = pinch.scale
			}
		}

		MouseArea {
			id: mouse
			anchors.fill: parent
			property int lastX: 0
			property int lastY: 0
			acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton

			onPressed: { lastX=mouse.x; lastY=mouse.y }

			onPositionChanged: {
				view.translate(mouse.x-lastX, mouse.y-lastY)

				lastX=mouse.x
				lastY=mouse.y
			}

			onWheel: {
				view.scale(wheel.angleDelta.y>0 ? 0.9 : 1.1, wheel.x, wheel.y)
			}
		}
	}
}
