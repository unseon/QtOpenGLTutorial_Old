import QtQuick 2.6
import QtQuick.Window 2.2
import OpenGLTutorial 1.0

Window {
    visible: true

    RenderSurface {

    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }

    Text {
        text: qsTr("Hello World")
        anchors.centerIn: parent
    }
}
