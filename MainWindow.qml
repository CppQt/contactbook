import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2

Window {
    title: qsTr("Contact book")

    ColumnLayout {
        anchors.fill: parent

        ListView {
            id: contactsView
            model: contactModel

            Layout.fillHeight: true
            Layout.fillWidth: true

            delegate: Rectangle {
                width: contactsView.width
                height: 50

                RowLayout {
                    anchors.fill: parent

                    Label {
                        id: firstNameLabel
                        Layout.preferredWidth: 50
                        text: model.firstName
                    }

                    Label {
                        id: lastNameLabel
                        Layout.preferredWidth: 50
                        text: model.lastname
                    }

                    Label {
                        id: birthdayLabel
                        Layout.preferredWidth: 50
                        text: model.birthday.toString()
                    }

                    Label {
                        id: emailLabel
                        Layout.preferredWidth: 50
                        text: model.email
                    }
                }
            }
        }

        RowLayout {
            Button {
                id: saveButton
                text: qsTr("Save")
                Layout.fillWidth: true
                onClicked: {
                    console.log("Save")
                    fileDialog.selectExisting = false
                    fileDialog.open()
                }
            }
            Button {
                id: loadButton
                text: qsTr("Load")
                Layout.fillWidth: true
                onClicked: {
                    fileDialog.selectExisting = true
                    fileDialog.open()
                }
            }
            Button {
                id: exitButton
                text: qsTr("Exit")
                Layout.fillWidth: true
                onClicked: Qt.quit()
            }
        }
    }

    MessageDialog {
        id: messageDialog
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Ok
    }

    FileDialog {
        id: fileDialog
        title: selectExisting ? qsTr("Choose file to load") : qsTr("Enter file to save")
        selectFolder: false

        width: 400
        height: 200

        folder: shortcuts.home
        onAccepted: {
            console.log("Selected file:", fileDialog.fileUrls)
            var fileName = fileDialog.fileUrls[0]
            if (fileName !== null && fileName !== "") {
                if (fileDialog.selectExisting) {
                    if (!contactModel.loadData(fileName)) {
                        messageDialog.text = qsTr("Error when loading data")
                        messageDialog.open()
                    }
                } else {
                    if (!contactModel.saveData(fileName)) {
                        messageDialog.text = qsTr("Error when saving data")
                        messageDialog.open()
                    }
                }
            }
        }
        onRejected: {
            console.log("Canceled select file")
        }
    }

    EditRecordDialog {
        id: editRecordDialog
    }
}
