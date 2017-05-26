import QtQuick 2.5
import QtQuick.Controls 1.4

ApplicationWindow {
    id: window
    visible: true
    width: 1280
    height: 1024
    title: qsTr("Moving Blocks")
    color: "transparent"

    Item {
        id: i
        anchors.fill: parent

    property int blockWidth: 30
    property int moveDuration: 1000
    property int elapsedSecs: 0

    states: [
        State {
            name: "state1"
            when: (i.elapsedSecs % 6) >= 0 && (i.elapsedSecs % 6) < 2
            PropertyChanges {
                target: redRect
                x: 100; y: 400
            }
            PropertyChanges {
                target: blueRect
                x: 200; y: 400
            }
            PropertyChanges {
                target: yellowRect
                x: 100; y: 200
            }
        },
        State {
            name: "state2"
            when: (i.elapsedSecs % 6) >= 2 && (i.elapsedSecs % 6) < 4
            PropertyChanges {
                target: redRect
                x: 100; y: 600
            }
            PropertyChanges {
                target: blueRect
                x: 100; y: 200
            }
            PropertyChanges {
                target: yellowRect
                x: 400; y: 600
            }
        },
        State {
            name: "state3"
            when: (i.elapsedSecs % 6) >= 4 && (i.elapsedSecs % 6) < 6
            PropertyChanges {
                target: redRect
                x: 400; y: 600
            }
            PropertyChanges {
                target: blueRect
                x: 100; y: 600
            }
            PropertyChanges {
                target: yellowRect
                x: 400; y: 200
            }
        }
    ]
    state: "state1"

    Timer {
        interval: 1000
        repeat: true
        running: true
        onTriggered: i.elapsedSecs++
    }

    Rectangle {
        id: redRect
        color: "red"
        width: i.blockWidth
        height: i.blockWidth
        Behavior on x {
            NumberAnimation {
                duration: i.moveDuration
            }
        }
        Behavior on y {
            NumberAnimation {
                duration: i.moveDuration
            }
        }
    }

    Rectangle {
        id: blueRect
        color: "blue"
        width: i.blockWidth
        height: i.blockWidth
        Behavior on x {
            NumberAnimation {
                duration: i.moveDuration
            }
        }
        Behavior on y {
            NumberAnimation {
                duration: i.moveDuration
            }
        }
    }

    Rectangle {
        id: yellowRect
        color: "yellow"
        width: i.blockWidth
        height: i.blockWidth
        Behavior on x {
            NumberAnimation {
                duration: i.moveDuration
            }
        }
        Behavior on y {
            NumberAnimation {
                duration: i.moveDuration
            }
        }
    }
    }

}
