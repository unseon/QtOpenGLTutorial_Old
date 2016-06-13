import QtQuick 2.0
import QtQuick.Window 2.0
import OpenGLTutorial 1.0

Window {
    visible: true

    width: 400
    height: 400

    RenderSurface {
        id: renderSurface
        anchors.fill: parent
    }

    Text {
        text: "FPS: " + renderSurface.fps.toFixed(2)
        anchors.top: parent.top
        anchors.left: parent.left
        color: "white"
        font.pixelSize: 50
    }
}
