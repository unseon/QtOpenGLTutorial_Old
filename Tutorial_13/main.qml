import QtQuick 2.0
import QtQuick.Window 2.0
import OpenGLTutorial 1.0

Window {
    visible: true

    width: 400
    height: 400

    Rectangle {
        id: frame
        anchors.fill: parent
        color: "red"

        Grid {
            columns: 2
            spacing: 2

            RenderSurface {
                id: renderSurface
                width: frame.width / 2
                height: frame.height / 2
            }

            RenderSurface {
                width: frame.width / 2
                height: frame.height / 2
            }

            RenderSurface {
                width: frame.width / 2
                height: frame.height / 2
            }

            RenderSurface {
                width: frame.width / 2
                height: frame.height / 2
            }
        }
    }

    Text {
        text: "FPS: " + renderSurface.fps.toFixed(2)
        anchors.top: parent.top
        anchors.left: parent.left
        color: "white"
        font.pixelSize: 50
    }
}
