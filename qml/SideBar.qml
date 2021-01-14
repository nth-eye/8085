import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    property alias idx: list.currentIndex

    border.width: 1
    border.color: colors.dark_3
    color: colors.dark_1

    ListModel {
        id: model
        ListElement {
            src: "qrc:/qml/img/icon_asm.png"
            chk: true
        }
        ListElement {
            src: "qrc:/qml/img/icon_bug.png"
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
