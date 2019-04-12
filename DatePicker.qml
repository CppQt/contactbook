import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import Qt.labs.calendar 1.0

Dialog {
    id: calendar
    width: 310
    height: 260

    property date selectedDate: new Date()

    function popup(date, pos) {
        if (pos !== null) {
            calendar.x = pos.x
            calendar.y = pos.y
        }
        if (date) {
            selectedDate = date
        }

        calendar.open()
    }

    function datesEqual(left, right) {
        return left.getDate() === right.getDate() &&
                left.getMonth() === right.getMonth() &&
                left.getFullYear() === right.getFullYear()
    }

    function fromFuture(date) {
        var now = new Date()
        var test = new Date(date.getFullYear(), date.getMonth(), date.getDate())
        return test > now
    }

    ListModel {
        id: years
        ListElement { key: 1900; value: 1900 }
    }

    ListModel {
        id: months
        ListElement { key: qsTr("January");    value: Calendar.January     }
        ListElement { key: qsTr("Ferbruary");  value: Calendar.February    }
        ListElement { key: qsTr("March");      value: Calendar.March       }
        ListElement { key: qsTr("April");      value: Calendar.April       }
        ListElement { key: qsTr("May");        value: Calendar.May         }
        ListElement { key: qsTr("June");       value: Calendar.June        }
        ListElement { key: qsTr("July");       value: Calendar.July        }
        ListElement { key: qsTr("August");     value: Calendar.August      }
        ListElement { key: qsTr("September");  value: Calendar.September   }
        ListElement { key: qsTr("October");    value: Calendar.October     }
        ListElement { key: qsTr("November");   value: Calendar.November    }
        ListElement { key: qsTr("December");   value: Calendar.December    }
    }

    onVisibleChanged: {
        if (visible) {
            var isValid = selectedDate && !Number.isNaN(selectedDate.getTime())
            var date = isValid ? selectedDate : new Date()
            year.value =  date.getFullYear()
            month.currentIndex = date.getMonth()
        }
    }

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            SpinBox {
                id: year
                from: 1900
                to: new Date().getFullYear()
                editable: true
            }

            ComboBox {
                id: month
                model: months
                textRole: "key"
            }
        }

        GridLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            columns: 2

            DayOfWeekRow {
                locale: grid.locale

                Layout.column: 1
                Layout.fillWidth: true
            }

            WeekNumberColumn {
                month: grid.month
                year: grid.year
                locale: grid.locale

                Layout.fillHeight: true
            }

            MonthGrid {
                id: grid
                month: months.get(month.currentIndex).value
                year: year.value
                locale: Qt.locale()

                delegate: Rectangle {
                    property bool selected: datesEqual(model.date, calendar.selectedDate)
                    color: selected ? "gray" : "transparent"
                    border.color: model.today ? "blue" : "transparent"
                    enabled: !fromFuture(model.date)
                    Text {
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        opacity: model.month === grid.month ? 1 : 0.4
                        text: model.day
                        font: grid.font
                        color: fromFuture(model.date) ? "gray" : "black"
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            calendar.selectedDate = model.date
                            calendar.accept()
                        }
                    }
                }

                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}
