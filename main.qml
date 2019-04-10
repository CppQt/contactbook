import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Contact book")

    Button {
        text: "Open test dialog"
        onClicked: testDialog.open()
    }

    Dialog {
        id: testDialog
    }
}
