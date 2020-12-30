import QtQuick 2.0
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.15
import QtQml.Models 2.15

import backend.settings 1.0

Dialog {
    modal: true
    dim: false
    closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape
    title: "Settings"
    standardButtons: Dialog.Ok | Dialog.RestoreDefaults | Dialog.Cancel

    property var old_settings: []

    onOpened: {
        for (var i = 0; i < list_model.rowCount(); ++i)
            old_settings[i] = String(list.itemAtIndex(i).color)
    }
    onAccepted: {
        old_settings = []
    }
    onRejected: {
        for (var i = 0; i < old_settings.length; ++i) {
            Settings.set_value(list.itemAtIndex(i).text, old_settings[i]);
            list.itemAtIndex(i).color = old_settings[i]
        }
        Settings.update();
        old_settings = []
    }
    onReset: {
        Settings.reset();
        for (var i = 0; i < list_model.rowCount(); ++i)
            list.itemAtIndex(i).color = Settings.value(list.itemAtIndex(i).text)
    }

    ColorDialog {
        property string selected_setting

        id: color_dialog
        title: "Please choose a color"
        onAccepted: {
            selected_setting = list_model.data(list_model.index(list.currentIndex, 0));
            list.currentItem.color = color_dialog.color;
            Settings.set_value(selected_setting, color_dialog.color);
            Settings.update();
        }
    }

    ListView {
        id: list
        interactive: false
        snapMode: ListView.SnapOneItem
        orientation: ListView.Vertical
        model: list_model
        anchors.fill: parent
        spacing: 4
        delegate: RowLayout {

            width: list.width / 2
            property alias color: rect.color
            property alias text: lbl.text

            Label {
                id: lbl
                Layout.alignment: Qt.AlignLeft
                text: display
            }

            Rectangle {
                id: rect
                Layout.alignment: Qt.AlignRight
                color: Settings.color_value(display)
                width: 240
                height: 20
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        list.currentIndex = index
                        color_dialog.open()
                    }
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
    }
}
