import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2

Window {
    title: qsTr("Contact list")

    ColumnLayout {
        anchors.fill: parent
        ListView {
            id: contactsView
            model: contactModel

            Layout.fillHeight: true
            Layout.fillWidth: true

            boundsBehavior: Flickable.StopAtBounds

            ScrollBar.vertical: ScrollBar {}

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
                            editRecordDialog.model = model
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
                            editRecordDialog.model = model
                            editRecordDialog.open()
                        }
                    }
                    MenuItem {
                        text: qsTr("Insert contact before")
                        onTriggered: {
                            editRecordDialog.resetFields()
                            editRecordDialog.isBefore = true
                            editRecordDialog.row = index
                            editRecordDialog.open()
                        }
                    }
                    MenuItem {
                        text: qsTr("Insert contact after")
                        onTriggered: {
                            editRecordDialog.resetFields()
                            editRecordDialog.isBefore = false
                            editRecordDialog.row = index
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
                        editRecordDialog.open()
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
            var fileName = fileDialog.fileUrls[0].replace(/^file:\/\//, "")
            console.log("Selected file:", fileName)
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
        width: 200
        height: 100

        property bool isBefore: true
        property int row: -1

        onAccepted: {
            if (model) {
                model.firstName = firstName
                model.lastName = lastName
                model.birthday = Date.fromLocaleDateString(Qt.locale(), birthday, Locale.ShortFormat)
                model.email = email
            } else {
                var date = Date.fromLocaleDateString(Qt.locale(), birthday, Locale.ShortFormat)
                if (isBefore) {
                    contactModel.addRowBefore(row, firstName, lastName, date, email)
                } else {
                    contactModel.addRowAfter(row, firstName, lastName, date, email)
                }
            }
        }
    }
}
