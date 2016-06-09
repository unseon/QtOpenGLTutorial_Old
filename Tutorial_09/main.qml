import QtQuick 2.0
import QtQuick.Window 2.0
import OpenGLTutorial 1.0

Window {
    visible: true

    width: 400
    height: 400

    RenderSurface {
        anchors.fill: parent
    }

//    MouseArea {
//        anchors.fill: parent
//        onClicked: {
//            Qt.quit();
//        }
//    }

    Text {
        text: qsTr("Hello World")
        anchors.centerIn: parent
    }
}
