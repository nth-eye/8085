import QtQuick 2.15
import QtQuick.Controls 2.15

TabButton {
    property string border_color: colors.light_5
    property alias right_border: border_right
    property alias left_border: border_left
    property alias top_border: border_top
    property alias bottom_border: border_bottom

    icon.color: hovered || checked ? colors.light_4 : colors.light_1

    MouseArea {
        id: mouse_area
        anchors.fill: parent
        onPressed: mouse.accepted = false
        cursorShape: Qt.PointingHandCursor
    }

    background: Rectangle {
        id: background
        color: "transparent"
    }

    Rectangle {
        id: border_right
        color: border_color
        width: 1
        anchors.right: parent.right
        height: parent.height
        visible: true
    }

    Rectangle {
        id: border_left
        color: border_color
        width: 1
        anchors.left: parent.left
        height: parent.height
        visible: true
    }

    Rectangle {
        id: border_top
        color: border_color
        height: 1
        anchors.top: parent.top
        width: parent.width
        visible: true
    }

    Rectangle {
        id: border_bottom
        color: border_color
        height: 1
        anchors.bottom: parent.bottom
        width: parent.width
        visible: true
    }
}
