import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2 as Dialogs
import QtQuick.Layouts 1.12

Dialogs.Dialog {
    id: dialog
    title: qsTr("Edit line")

    property alias firstName: firstNameInput.text
    property alias lastName: lastNameInput.text
    property alias birthday: birthdayInput.text
    property alias email: emailInput.text

    property bool edit: false
    property bool isBefore: true
    property int row: -1

    function resetFields() {
        firstName = ""
        lastName = ""
        birthday = ""
        email = ""
        edit = false
    }

    GridLayout {
        anchors.fill: parent

        Label {
            Layout.column: 0
            Layout.row: 0
            text: qsTr("First name")
        }
        TextField {
            Layout.column: 1
            Layout.row: 0
            id: firstNameInput
            selectByMouse: true
            color: acceptableInput ? "black" : "red"
            validator: RegExpValidator { regExp: /[^\s]+/ }
        }

        Label {
            Layout.column: 0
            Layout.row: 1
            text: qsTr("Last name")
        }
        TextField {
            Layout.column: 1
            Layout.row: 1
            id: lastNameInput
            selectByMouse: true
            color: acceptableInput ? "black" : "red"
            validator: RegExpValidator { regExp: /[^\s]+/ }
        }

        Label {
            Layout.column: 0
            Layout.row: 2
            text: qsTr("Birthday")
        }
        TextField {
            Layout.column: 1
            Layout.row: 2
            id: birthdayInput
            selectByMouse: true
            inputMethodHints: Qt.ImhDate
            color: acceptableInput ? "black" : "red"
        }

        Label {
            Layout.column: 0
            Layout.row: 3
            text: qsTr("Email")
        }
        TextField {
            Layout.column: 1
            Layout.row: 3
            id: emailInput
            selectByMouse: true
            inputMethodHints: Qt.ImhEmailCharactersOnly
            color: acceptableInput ? "black" : "red"
            validator: RegExpValidator { regExp: new RegExp(contactModel.emailValidator()) }
        }
        DialogButtonBox {
            Layout.column: 1
            Layout.row: 4
            Layout.columnSpan: 2
            buttonLayout: DialogButtonBox.WinLayout
            padding: 0
            Button {
                id: saveButton
                text: qsTr("Ok")
                enabled: firstNameInput.acceptableInput &&
                         lastNameInput.acceptableInput &&
                         birthdayInput.acceptableInput &&
                         emailInput.acceptableInput
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            }
            Button {
                id: loadButton
                text: qsTr("Cancel")
                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            }
            onAccepted: {
                dialog.accepted()
                dialog.close()
            }
            onRejected: {
                dialog.rejected()
                dialog.close()
            }
        }
    }
    standardButtons: DialogButtonBox.NoButton
}
