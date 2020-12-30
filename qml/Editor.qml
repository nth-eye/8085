import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3

import backend.doc_handler 1.0

SplitView {
//    property alias tabs: stack.children

    orientation: Qt.Vertical

    handle: Rectangle {
        implicitWidth: 4
        implicitHeight: 4
        color: colors.light_1
    }

//    background: Rectangle {
//        color: colors.dark_2
//    }

//    MessageDialog {
//        id: dialog
//    }

//    Component {
//        id: tab_button_component
//        MyButton {
//            property int idx

//            id: tab_button
//            text: "tab"
//            font.pixelSize: 14
//            bottom_border.height: 2
//            bottom_border.visible: checked ? true : false
//            left_border.visible: false
//            right_border.visible: false
//            top_border.visible: false
//            contentItem: RowLayout {
//                spacing: 0
//                Text {
//                    id: text
//                    text: tab_button.text
//                    font: tab_button.font
//                    color: tab_button.hovered || tab_button.checked ? colors.light_4 : colors.light_1
//                    horizontalAlignment: Text.AlignHCenter
//                    verticalAlignment: Text.AlignVCenter
//                    elide: Text.ElideRight
//                }
//                Item {
//                    Layout.leftMargin: 0
//                    Layout.rightMargin: -5
//                    width: 26
//                    height: 26
//                    Button {
//                        anchors.fill: parent
//                        icon.source: "img/icon_close_2.png"
//                        icon.color: tab_button.checked ? colors.light_4 : colors.light_1
//                        icon.width: 64
//                        icon.height: 64
//                        background: Rectangle {
//                            color: "transparent"
//                        }
//                        visible: tab_button.hovered || tab_button.checked ? true : false
//                        onClicked: {
//                            stack.children[idx].destroy();
//                            bar.removeItem(this.parent.parent.parent);
//                        }
//                        MouseArea {
//                            id: mouse_area
//                            anchors.fill: parent
//                            onPressed: mouse.accepted = false
//                            cursorShape: Qt.PointingHandCursor
//                        }
//                    }
//                }
//            }
//        }
//    }

//    Component {
//        id: editor_window_component
//        EditorWindow {
//            property int idx

//            id: editor_window
//        }
//    }

//    ColumnLayout {
//        anchors.fill: parent
//        Rectangle {
//            color: colors.dark_1
//            Layout.fillWidth: true
//            height: bar.height
//            visible: bar.currentIndex != -1
//            TabBar {
//                id: bar
//                background: Rectangle {
//                    color: "transparent"
//                    anchors.fill: parent
//                }
//            }
//        }
//        StackLayout {
//            id: stack
//            width: parent.width
//            currentIndex: bar.currentIndex
//        }
//    }

    Rectangle {
        SplitView.preferredHeight: parent.height / 2
        color: "transparent"
    }

    Rectangle {
        SplitView.preferredHeight: parent.height / 2
        color: colors.dark_1
    }

//    function remove_tab(idx)
//    {
//    }

//    function add_tab(file_name)
//    {
////        for (var i = 0;  i < stack.children.length; ++i)
////            console.log(stack.children[i]);

//        var tab = tab_button_component.createObject(
//                    bar,
//                    {
//                        text: file_name,
//                        idx: bar.currentIndex + 1
//                    }
//        );
//        bar.addItem(tab);

//        var item = editor_window_component.createObject(
//                    stack,
//                    {
//                        id: bar.currentIndex,
//                        idx: bar.currentIndex
//                    }
//        );
//        stack.children.push(item)
//    }

//    function active_doc()
//    {
//        return stack.visibleChildren[0] ? stack.visibleChildren[0].doc : "none";
//    }
}
