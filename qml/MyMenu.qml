import QtQuick 2.15
import QtQuick.Controls 2.15

Menu {

    delegate: MenuItem {
        id: menu_item

        contentItem: Text {
            text: menu_item.text
            font: menu_item.font
            color: colors.light_4
//            horizontalAlignment: Text.AlignLeft
//            verticalAlignment: Text.AlignVLeft
            elide: Text.ElideRight
        }

        background: Rectangle {
            color: menu_item.highlighted ? colors.dark_4 : colors.dark_2
        }
    }
}
