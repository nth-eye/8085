import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    property alias idx: list.currentIndex

    border.width: 1
    border.color: colors.dark_3
    color: colors.dark_1

    ListModel {
        id: model
        ListElement {
            src: "img/icon_asm.png"
            chk: true
        }
        ListElement {
            src: "img/icon_bug.png"
            chk: false
        }
    }

    ListView {
        id: list
        interactive: false
        snapMode: ListView.SnapOneItem
        orientation: ListView.Vertical
        model: model
        anchors.fill: parent
        delegate: MyButton {
            icon.source: src
            icon.height: list.width
            icon.width: list.width
            width: list.width
            left_border.width: 2
            left_border.visible: checked ? true : false
            right_border.visible: false
            top_border.visible: false
            bottom_border.visible: false
            checked: chk
            onClicked: list.currentIndex = index
        }
    }
}
