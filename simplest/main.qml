import QtQuick 2.7
import QtQuick.Controls 2.0
import com.l3 1.0;

ApplicationWindow {
    visible: true
    width: 1280
    height: 240
    title: qsTr("Simplest")
    id: root

    PaintTimeLogger {
        id: ptLogger
        anchors.fill: parent;
        Rectangle {
            id: rect
            anchors.fill: parent
            color: "#0000ff"
            visible: true

//            Text {
//                anchors.centerIn: parent
//                color: "black"
//                text: ptLogger.averagePaintTime
//            }

            Timer {
                interval: 100
                repeat: true
                running: true
                onTriggered: rect.color = (rect.color == "#0000ff" ? "#ff0000" : "#0000ff")
            }
        }
    }
}
