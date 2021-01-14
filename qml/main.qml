import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
// import Qt.labs.settings 1.0
import Qt.labs.platform 1.1

import backend.settings 1.0

ApplicationWindow {

    id: window

    Colors {
        id: colors
    }

    width: 1024
    height: 600
    visible: true
    visibility: ApplicationWindow.Maximized
    title: qsTr("8085-qml")
    // flags: Qt.FramelessWindowHint

    function basename(str)
    {
        return (str.slice(str.lastIndexOf("/")+1))
    }


    menuBar: MyMenuBar {

        MyMenu {
            title: qsTr("File")
            font.pixelSize: parent.font.pixelSize

            Action {
                text: qsTr("New File")
                shortcut: "Ctrl+N"
                icon.name: "edit-copy"
                onTriggered: editor.add_tab("Untitled")// window.activeFocusItem.copy()
            }
            Action {
                text: qsTr("Open File...")
                shortcut: "Ctrl+O"
                onTriggered: open_dialog.open()
            }
            MyMenuSeparator { }
            Action {
                text: qsTr("Save")
                shortcut: "Ctrl+S"
                onTriggered: save_dialog.document.save(file)
            }
            Action {
                text: qsTr("Save As...")
                shortcut: "Ctrl+Shift+S"
                onTriggered: save_dialog.open()
            }
            MyMenuSeparator { }
            Action {
                text: qsTr("Settings")
                onTriggered: settings_window.open()
            }
            MyMenuSeparator { }
            Action {
                text: qsTr("Exit")
                onTriggered: exit_dialog.open()
            }
        }
        MyMenu {
            title: qsTr("Edit")
            font.pixelSize: parent.font.pixelSize

            Action { text: qsTr("Cut") }
            Action { text: qsTr("Copy") }
            Action { text: qsTr("Paste") }
        }
        MyMenu {
            title: qsTr("Help")
            font.pixelSize: parent.font.pixelSize

            Action { text: qsTr("About") }
        }
    }

    FileDialog {
         id: open_dialog
         nameFilters: ["8085 files (*.a85 *.asm)", "Text files (*.txt)"]
         folder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
         onAccepted: {
             editor.add_tab(basename(fileUrl.toString()));
             editor.active_doc().load(fileUrl);
         }
    }

    FileDialog {
        id: save_dialog
        nameFilters: ["8085 files (*.a85 *.asm)", "Text files (*.txt)", "All files (*)"]
        // selectExisting: false
        onAccepted: document.save(file)
    }

    MessageDialog {
        id : exit_dialog
        title: qsTr("Exit?")
        text: qsTr("The file has been modified. Exit anyway?")
        // icon: StandardIcon.Question
        buttons: MessageDialog.Yes | MessageDialog.No
        onYesClicked: Qt.quit()
        onNoClicked: console.log("didn't copy")
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        SideBar {
            id: side_bar
            width: 47
            Layout.fillWidth: false
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignBottom | Qt.AlignTop
        }

        Rectangle {
            color: colors.dark_2
            border.color: colors.dark_3
            border.width: 1
            Layout.fillHeight: true
            Layout.fillWidth: true

            StackLayout {
                anchors.fill: parent
                currentIndex: side_bar.idx

                Editor {
                    id: editor
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                Debugger {
                    id: debugger_tab
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }
        }
    }

    SettingsWindow {
        id: settings_window
        anchors.centerIn: parent
        width: 800
        height: 640
//        onSettingsChanged: {
//            for (var i = 0; i < editor.tabs.length; ++i)
//                editor.tabs[i].doc.update();
//        }
    }

    Connections {
        target: Settings
        function onSettingsChanged() {
            for (var i = 0; i < editor.tabs.length; ++i)
                editor.tabs[i].doc.update();
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
