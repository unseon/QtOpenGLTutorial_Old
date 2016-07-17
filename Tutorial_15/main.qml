import QtQuick 2.0
import QtQuick.Window 2.0
import OpenGLTutorial 1.0

Window {
    visible: true

    width: 400
    height: 400

    RenderSurface {
        id: renderSurface
        width: 200
        height: 200
    }

    RenderSurface {
        x: 200
        y: 200
        width: 200
        height: 200
    }

    Text {
        text: "Hello Qt"
        anchors.top: parent.top
        anchors.left: parent.left
        color: "white"
        font.pixelSize: 50
    }

}
