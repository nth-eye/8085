import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import backend.simulator 1.0
import backend.settings 1.0

SplitView {

    orientation: Qt.Vertical

    handle: Rectangle {
        implicitWidth: 8
        implicitHeight: 8
        color: "transparent"
    }

    Rectangle {
        id: upper_part
        SplitView.preferredHeight: parent.height / 2
        color: colors.dark_2
        Rectangle {
            id: control_bar
            width: parent.width
            height: 28
            color: colors.dark_1
            border.color: colors.dark_3
            border.width: 1
        }
        Rectangle {
            id: program
            color: "transparent"
            border.width: 2
            border.color: colors.light_1
            anchors.top: control_bar.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: registers.left
            anchors.margins: 10
            anchors.bottomMargin: 0
            Button {
                width: 200
                height: 100
                anchors.centerIn: parent
                text: "assemble demo"
                onClicked: Simulator.assemble("../8085-qml/example_bubble_sort.asm")
            }
        }
        Rectangle {
            id: registers
            color: "transparent"
            border.width: 2
            border.color: colors.light_1
            anchors.top: control_bar.bottom
            anchors.right: flags.left
            height: format.height + reg_list.height + 20
            width: reg_lbl.width + format.width + 22
            anchors.margins: 10
            Text {
                id: reg_lbl
                text: "Reg"
                color: colors.light_1
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.topMargin: 5
                anchors.leftMargin: 10
                height: format.height
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            ComboBox {
                id: format
                font.pixelSize: 14
                width: 90
                height: 30
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 6
                model: ["Hex", "Dec", "Bin"]
                background: Rectangle {
                    color: "transparent"
                }
                delegate: ItemDelegate {
                    width: format.width
                    clip: true
                    background: Rectangle {
                        color: "transparent"
                    }
                    contentItem: Text {
                        text: modelData
                        color: Settings.value("editor/unit")
                        font: format.font
                        verticalAlignment: Text.AlignVCenter
                    }
                    highlighted: format.highlightedIndex === index
                }
                contentItem: Text {
                    text: format.displayText
                    font: format.font
                    color: Settings.value("editor/unit")
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
                onCurrentIndexChanged: Simulator.format = currentIndex
            }
            ListView {
                property var reg_names: ["A", "B", "C", "D", "E", "H", "L", "M", "SP", "PC"]
                id: reg_list
                anchors.top: format.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 20
                anchors.topMargin: 0
                anchors.rightMargin: 10
                interactive: false
                height: contentHeight
                model: Simulator.reg_model
                delegate: RowLayout {
                    width: reg_list.width
                    height: 20
                    Label {
                        color: Settings.value("editor/register")
                        text: reg_list.reg_names[index]
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                        Layout.alignment: Qt.AlignLeft
                    }
                    ValueBox {
                        text: model.display
                        Layout.alignment: Qt.AlignRight
                    }
                }
            }
        }
        Rectangle {
            id: flags
            color: "transparent"
            border.width: 2
            border.color: colors.light_1
            anchors.top: control_bar.bottom
            anchors.right: parent.right
            height: registers.height
            width: 80
            anchors.margins: 10
            Text {
                id: flags_lbl
                text: "Flag"
                color: colors.light_1
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.topMargin: 5
                anchors.leftMargin: 10
                height: format.height
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            ListView {
                property var flags_names: ["CY", "-", "P", "-", "AC", "-", "Z", "S"]
                id: flags_list
                anchors.top: flags_lbl.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 20
                anchors.topMargin: 0
                anchors.rightMargin: 10
                interactive: false
                height: contentHeight
                model: Simulator.flags_model
                delegate: RowLayout {
                    width: flags_list.width
                    height: 20
                    Label {
                        color: Settings.value("editor/register")
                        text: flags_list.flags_names[index]
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                        Layout.alignment: Qt.AlignLeft
                    }
                    ValueBox {
                        metrics: TextMetrics { text: "0"; font: font }
                        text: model.display
                        Layout.alignment: Qt.AlignRight
                    }
                }
            }
        }
        Rectangle {
            id: other_info
            color: "transparent"
            border.width: 2
            border.color: colors.light_1
            anchors.top: registers.bottom
            anchors.bottom: parent.bottom
            anchors.left: program.right
            anchors.right: parent.right
            anchors.margins: 10
            anchors.bottomMargin: 0
            GridLayout {
                columns: 2
                anchors.top: parent.top
                // anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.margins: 20
                width: parent.width / 2 - 6
                Label {
                    color: Settings.value("editor/label")
                    text: "PC"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    Layout.alignment: Qt.AlignLeft
                }
                ValueBox {
                    metrics: TextMetrics { text: "0000"; font: font }
                    text: Simulator.pc.toString(16).padStart(4, '0').toUpperCase()
                    Layout.alignment: Qt.AlignRight
                }
                Label {
                    color: Settings.value("editor/label")
                    text: "SP"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    Layout.alignment: Qt.AlignLeft
                }
                ValueBox {
                    metrics: TextMetrics { text: "0000"; font: font }
                    text: Simulator.sp.toString(16).padStart(4, '0').toUpperCase()
                    Layout.alignment: Qt.AlignRight
                }
            }
        }
    }

    Rectangle {
        property var mono_font: "Monospace"
        property var font_size: 12
        property var row_space: 4
        property var col_space: 10

        id: lower_part
        SplitView.preferredHeight: parent.height / 2
        color: colors.dark_1

        Rectangle {
            id: stack
            color: "transparent"
            border.width: 2
            border.color: colors.light_1
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            width: 120
            anchors.margins: 10
            Text {
                id: stack_lbl
                text: "STACK"
                color: "white"
                font.bold: true
                font.family: lower_part.mono_font
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.topMargin: 20
                anchors.leftMargin: 20
                // height: format.height
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            Tumbler {
                id: stack_list
                anchors.top: stack_lbl.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.rightMargin: 20
                anchors.leftMargin: 20
                anchors.topMargin: 6
                anchors.bottomMargin: 20
                focus: true
                wheelEnabled: false
                enabled: false
                model: Simulator.stack_model
                visibleItemCount: (height / 22) & 1 ? (height / 22) : (height / 22) + 1
                delegate: RowLayout {
                    Label {
                        text: model.address
                        font.family: lower_part.mono_font
                        color: index == stack_list.currentIndex ? Settings.value("editor/label") : "gray"
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                        Layout.alignment: Qt.AlignLeft
                    }
                    ValueBox {
                        text: model.byte
                        metrics: TextMetrics { text: "00"; font: font }
                        Layout.alignment: Qt.AlignRight
                    }
                }
                currentIndex: Simulator.sp
            }
//            MouseArea {
//                anchors.fill: parent
//                onWheel: {
//                    if (wheel.angleDelta.y > 0)
//                        stack_list.currentIndex -= 1;
//                    else
//                        stack_list.currentIndex += 1;
//                }
//            }
        }

        Rectangle {
            color: "transparent"
            border.width: 2
            border.color: colors.light_1
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: stack.right
            width: content.width + 20
            anchors.margins: 10

            GridLayout {
                id: content
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.margins: 20
                anchors.topMargin: 20
                columns: 3
                rows: 2
                columnSpacing: 24
                rowSpacing: 8

                Label {
                    text: "OFFSET"
                    color: "white"
                    font.bold: true
                    font.family: lower_part.mono_font
                }
                Row {
                    spacing: lower_part.col_space
                    Repeater {
                        model: 16
                        Label {
                            text: index.toString(16).padStart(2, '0').toUpperCase()
                            color: "white"
                            font.bold: true
                            font.family: lower_part.mono_font
                        }
                    }
                }
                Label {
                    text: "DECODED TEXT"
                    color: "white"
                    font.bold: true
                    font.family: lower_part.mono_font
                }
                ListView {
                    id: offset
                    interactive: false
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    boundsMovement: Flickable.StopAtBounds
                    boundsBehavior: Flickable.StopAtBounds
                    clip: true
                    spacing: 3
                    model: Simulator.memory_model
                    delegate: Label {
                        text: model.offset
                        font.family: lower_part.mono_font
                        color: "gray"
                    }
                    contentY: mem_scrollbar.position * contentHeight
                }
                TableView {
                    id: bytes
                    interactive: false
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    boundsMovement: Flickable.StopAtBounds
                    boundsBehavior: Flickable.StopAtBounds
                    clip: true
                    columnSpacing: lower_part.col_space
                    rowSpacing: 3
                    model: Simulator.memory_model
                    delegate: Label {
                        text: model.byte
                        font.family: lower_part.mono_font
                        color: "gray"
                    }
                    contentY: mem_scrollbar.position * contentHeight
                }
                TableView {
                    id: ascii
                    interactive: false
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    width: 330
                    boundsMovement: Flickable.StopAtBounds
                    boundsBehavior: Flickable.StopAtBounds
                    clip: true
                    columnSpacing: lower_part.col_space
                    rowSpacing: 3
                    model: Simulator.memory_model
                    delegate: Text {
                        text: model.char
                        font.family: lower_part.mono_font
                        color: "gray"
                    }
                    contentY: mem_scrollbar.position * contentHeight
                }
            }
            MouseArea {
                anchors.fill: parent
                onWheel: {
                    var new_pos = mem_scrollbar.position - (wheel.angleDelta.y / 8 /*5.46*/) / offset.contentHeight;
                    if (new_pos < 0.0)
                        new_pos = 0.0
                    if (new_pos >= 0.996)
                        new_pos = 0.996
                    mem_scrollbar.position = new_pos;
                }
            }
            ScrollBar {
                id: mem_scrollbar
                hoverEnabled: true
                active: hovered || pressed
                orientation: Qt.Vertical
                width: 16
                anchors.right: content.right
                anchors.top: content.top
                anchors.bottom: content.bottom
                anchors.rightMargin: 4
            }
        }
    }
}
