import QtQuick
import QtQuick.Controls

MenuSeparator {
    background: Rectangle {
        color: colors.dark_2
        anchors.fill: parent
    }
    contentItem: Rectangle {
        color: colors.light_2
        implicitWidth: 200
        implicitHeight: 1
        height: 2
    }
}
