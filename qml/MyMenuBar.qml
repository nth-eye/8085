import QtQuick 2.15
import QtQuick.Controls 2.15

MenuBar {
   property string background_color: "#1a1a20"
   property string foreground_color: "#696c7e"
   property string border_color: "#1e1e25"

   font.pixelSize: 12

    delegate: MenuBarItem {

        contentItem: Text {
            text: parent.text
            font: parent.font
            opacity: enabled ? 1.0 : 0.3
            color: foreground_color
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        background: Rectangle {
            color: parent.highlighted ? "#22ffffff" : "transparent"
        }
    }

    background: Rectangle {
        color: background_color
    }
}
