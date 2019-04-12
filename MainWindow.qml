import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2 as Dialogs

import data.model 1.0

Window {
    title: qsTr("Contact list")

    property bool loading: false

    function showWarning(message) {
        messageDialog.text = message
        messageDialog.open()
    }

    RowLayout {
        anchors.fill: parent
        visible: loading
        Item {
            Layout.fillWidth: true
        }
        ColumnLayout {
            Layout.fillWidth: true
            Item {
                Layout.fillHeight: true
            }

            Label {
                text: qsTr("Loading...")
                Layout.alignment: Qt.AlignHCenter
            }

            Item {
                id: spinner
                AnimatedImage {
                    anchors.fill: parent
                    source: "qrc:/images/spinner.gif"
                    playing: loading
                }
                Layout.preferredHeight: 100
                Layout.preferredWidth: 100
                Layout.alignment: Qt.AlignHCenter
            }

            Button {
                id: stopLoadingButton
                text: qsTr("Stop loading")
                Layout.alignment: Qt.AlignHCenter
                onClicked: contactModel.stopLoading()
            }

            Item {
                Layout.fillHeight: true
            }
        }
        Item {
            Layout.fillWidth: true
        }
    }

    ColumnLayout {
        anchors.fill: parent
        visible: !loading

        ListView {
            id: contactsView
            model: contactModel

            Layout.fillHeight: true
            Layout.fillWidth: true

            boundsBehavior: Flickable.StopAtBounds

            ScrollBar.vertical: ScrollBar {
                minimumSize: 0.1
            }

            delegate: Rectangle {
                width: contactsView.width
                height: 30

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
                        text: model.lastName
                    }

                    Label {
                        id: birthdayLabel
                        Layout.preferredWidth: 100
                        text: Qt.formatDate(model.birthday, Qt.DefaultLocaleShortDate)
                    }

                    Label {
                        id: emailLabel
                        Layout.preferredWidth: 100
                        text: model.email
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: {
                        contactsView.currentIndex = index
                        if (mouse.button === Qt.LeftButton) {
                            editRecordDialog.resetFields()
                            editRecordDialog.firstName = model.firstName
                            editRecordDialog.lastName = model.lastName
                            editRecordDialog.birthday = Qt.formatDate(model.birthday, Qt.DefaultLocaleShortDate)
                            editRecordDialog.email = model.email
                            editRecordDialog.edit = true
                            editRecordDialog.open()
                        } else if (mouse.button === Qt.RightButton) {
                            contextMenu.popup()
                        }
                    }
                }
                Menu {
                    id: contextMenu
                    MenuItem {
                        text: qsTr("Edit contact")
                        onTriggered: {
                            editRecordDialog.resetFields()
                            editRecordDialog.firstName = model.firstName
                            editRecordDialog.lastName = model.lastName
                            editRecordDialog.birthday = Qt.formatDate(model.birthday, Qt.DefaultLocaleShortDate)
                            editRecordDialog.email = model.email
                            editRecordDialog.edit = true
                            editRecordDialog.open()
                        }
                    }
                    MenuItem {
                        text: qsTr("Insert contact before")
                        onTriggered: {
                            editRecordDialog.resetFields()
                            editRecordDialog.isBefore = true
                            editRecordDialog.row = index
                            editRecordDialog.edit = false
                            editRecordDialog.open()
                        }
                    }
                    MenuItem {
                        text: qsTr("Insert contact after")
                        onTriggered: {
                            editRecordDialog.resetFields()
                            editRecordDialog.isBefore = false
                            editRecordDialog.row = index
                            editRecordDialog.edit = false
                            editRecordDialog.open()
                        }
                    }
                    MenuItem {
                        text: qsTr("Remove contact")
                        onTriggered: {
                            console.log("Removed row:", index)
                            contactModel.removeRow(index)
                        }
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                z: -1
                onClicked: {
                    if (mouse.button === Qt.RightButton) {
                        listMenu.popup()
                    }
                }
            }

            Menu {
                id: listMenu
                MenuItem {
                    text: qsTr("Add new contact")
                    onTriggered: {
                        editRecordDialog.resetFields()
                        editRecordDialog.isBefore = false
                        editRecordDialog.row = contactModel.rowCount() - 1
                        editRecordDialog.edit = false
                        editRecordDialog.open()
                    }
                }
            }
        }

        DialogButtonBox {
            Layout.fillWidth: true
            buttonLayout: DialogButtonBox.WinLayout
            padding: 0
            Button {
                id: exitButton
                text: qsTr("Exit")
                DialogButtonBox.buttonRole: DialogButtonBox.Ignore
                onClicked: Qt.quit()
            }
            Button {
                id: saveButton
                text: qsTr("Save")
                DialogButtonBox.buttonRole: DialogButtonBox.Save
                onClicked: {
                    console.log("Save")
                    fileDialog.selectExisting = false
                    fileDialog.open()
                }
            }
            Button {
                id: loadButton
                text: qsTr("Load")
                DialogButtonBox.buttonRole: DialogButtonBox.Reset
                onClicked: {
                    fileDialog.selectExisting = true
                    fileDialog.open()
                }
            }
        }
    }

    Dialogs.MessageDialog {
        id: messageDialog
        icon: Dialogs.StandardIcon.Warning
        standardButtons: Dialogs.StandardButton.Ok
    }

    Dialogs.FileDialog {
        id: fileDialog
        title: selectExisting ? qsTr("Choose file to load") : qsTr("Enter file to save")
        selectFolder: false

        width: 400
        height: 200

        folder: shortcuts.home
        onAccepted: {
            var fileName = fileDialog.fileUrls[0].replace(/^file:\/\//, "")
            console.log("Selected file:", fileName)
            if (fileName !== null && fileName !== "") {
                if (fileDialog.selectExisting) {
                    loading = true
                    if (!contactModel.loadData(fileName)) {
                        showWarning(qsTr("Error when loading data"))
                    }
                    loading = false
                } else {
                    if (!contactModel.saveData(fileName, true)) {
                        showWarning(qsTr("Error when saving data"))
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
        width: 360
        height: 280
        title: edit ? qsTr("Edit contant") : qsTr("New contact")
        parentWindow: mainWindow

        onAccepted: {
            var date = Date.fromLocaleDateString(Qt.locale(), birthday, Locale.ShortFormat)
            if (edit) {
                var index = contactModel.index(contactsView.currentIndex, 0)
                contactModel.setData(index, firstName, ContactModel.FirstNameRole)
                contactModel.setData(index, lastName, ContactModel.LastNameRole)
                contactModel.setData(index, date, ContactModel.BirthdayRole)
                contactModel.setData(index, email, ContactModel.EmailRole)
            } else {
                if (row === -1) {
                    contactModel.appendRow(firstName, lastName, date, email)
                } else if (isBefore) {
                    contactModel.addRowBefore(row, firstName, lastName, date, email)
                } else {
                    contactModel.addRowAfter(row, firstName, lastName, date, email)
                }
            }
        }
    }
}
