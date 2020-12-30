import QtQuick 2.15
import QtQuick.Controls 2.15

import backend.doc_handler 1.0

Item {
    // property alias text: text_edit.text

    property alias doc: document

    ListView {
        property TextMetrics text_metrics: TextMetrics { text: "99999"; font: text_edit.font }

        id: line_numbers
        model: text_edit.text.split(/\n/g)
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 10
        width: text_metrics.boundingRect.width
        clip: true
        snapMode: ListView.SnapToItem
        interactive: false

        delegate: Rectangle {
            width: line_numbers.width
            height: line_text.height
            color: colors.dark_2

            Text {
                id: line_number
                anchors.horizontalCenter: parent.horizontalCenter
                text: index + 1
                color: colors.light_1
                // font: textMetrics.font
            }
            Text {
                id: line_text
                width: flickable.width
                text: modelData
                font: text_edit.font
                visible: false
                wrapMode: Text.WordWrap
            }
        }
        onContentYChanged: {
            if (!moving)
                return
            flickable.contentY = contentY
        }
    }

    Item {
        anchors.left: line_numbers.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 10
        // anchors.fill: parent

        Flickable {
            id: flickable
            anchors.fill: parent
            flickableDirection: Flickable.VerticalFlick
            boundsBehavior: Flickable.StopAtBounds
            boundsMovement: Flickable.StopAtBounds
            contentWidth: text_edit.width
            contentHeight: text_edit.height

            TextEdit {
                id: text_edit
                // anchors.fill: parent
                width: flickable.width
                height: flickable.height
                color: colors.light_4
                focus: true
                selectByMouse: true
                persistentSelection: true
                wrapMode: Text.WordWrap
            }
            onContentYChanged: {
                if (line_numbers.moving)
                    return
                line_numbers.contentY = contentY
            }
            ScrollBar.vertical: ScrollBar {}
        }
    }

    DocHandler {
        id: document
        document: text_edit.textDocument
        cursorPosition: text_edit.cursorPosition
        selectionStart: text_edit.selectionStart
        selectionEnd: text_edit.selectionEnd
        // Component.onCompleted: document.load("qrc:/example_bubble_sort.asm")
        onLoaded: {
            text_edit.text = text
        }
    }
}
