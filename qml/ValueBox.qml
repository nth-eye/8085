import QtQuick

import backend.settings 1.0

Rectangle {
    property alias text: txt.text
    property alias font: txt.font
    property TextMetrics metrics: TextMetrics { text: "00000000"; font: txt.font }

    color: colors.dark_4

    width: metrics.width + 12
    height: metrics.height + 2

    border.width: 2
    border.color: colors.dark_2

    Text {
        id: txt
        font.family: "Monospace"
        color: Settings.value("editor/number")
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.verticalCenter: parent.verticalCenter
    }
}
