#include "contactmodel.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

const QString EMAIL_VALIDATOR = "(?:[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*|"
                                "\"(?:[\\x01-\\x08\\x0b\\x0c\\x0e-\\x1f\\x21\\x23-\\x5b\\x5d-\\x7f]|"
                                "\\\\[\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f])*\")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+"
                                "[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\\[(?:(?:(2(5[0-5]|[0-4][0-9])|1[0-9][0-9]|[1-9]?[0-9]))\\.){3}"
                                "(?:(2(5[0-5]|[0-4][0-9])|1[0-9][0-9]|[1-9]?[0-9])|[a-z0-9-]*[a-z0-9]:"
                                "(?:[\\x01-\\x08\\x0b\\x0c\\x0e-\\x1f\\x21-\\x5a\\x53-\\x7f]|\\\\[\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f])+)\\])";

ContactModel::ContactModel(QObject *parent) : QAbstractListModel(parent)
{
}

int ContactModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return contacts.size();
}

QVariant ContactModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= contacts.size())
        return QVariant();

    switch (static_cast<Roles>(role)) {
    case Roles::FirstNameRole:
        return contacts.at(index.row()).firstName;
    case Roles::LastNameRole:
        return contacts.at(index.row()).lastName;
    case Roles::BirthdayRole:
        return contacts.at(index.row()).birthday;
    case Roles::EmailRole:
        return contacts.at(index.row()).email;
    default:
        return QVariant();
    }
}

bool ContactModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() >= contacts.size())
        return false;

    switch (static_cast<Roles>(role)) {
    case Roles::FirstNameRole:
        contacts[index.row()].firstName = value.toString();
        break;
    case Roles::LastNameRole:
        contacts[index.row()].lastName = value.toString();
        break;
    case Roles::BirthdayRole:
        contacts[index.row()].birthday = value.toDate();
        break;
    case Roles::EmailRole:
        contacts[index.row()].email = value.toString();
        break;
    default:
        return false;
    }
    emit dataChanged(index, index, { role });
    return true;
}

Qt::ItemFlags ContactModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QHash<int, QByteArray> ContactModel::roleNames() const
{
    QHash<int, QByteArray> result;
    result[Roles::FirstNameRole] = "firstName";
    result[Roles::LastNameRole] = "lastName";
    result[Roles::BirthdayRole] = "birthday";
    result[Roles::EmailRole] = "email";
    return result;
}

bool ContactModel::loadData(const QString &fileName)
{
    qDebug() << "Open file:" << fileName;
    if (!QFile::exists(fileName))
        return false;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    beginResetModel();
    contacts.clear();
    QTextStream stream(&file);
    QString line;
    while (stream.readLineInto(&line)) {
        qDebug() << "Line:" << line;
        QStringList parts = line.split(QRegularExpression("\\s+"));
        if (parts.size() < 4) {
            qDebug() << "Not enough fields";
            continue;
        }

        QString firstName = parts.at(0);
        QString lastName = parts.at(1);
        QString dateString = parts.at(2);
        QDate date = QDate::fromString(dateString, Qt::ISODate);
        if (!date.isValid()) {
            qDebug() << "Date is invalid" << dateString;
            continue;
        }
        QString emailString = parts.at(3);
        QRegularExpression vRegExp(EMAIL_VALIDATOR);
        if (!vRegExp.match(emailString).hasMatch()) {
            qDebug() << "Email is invalid" << emailString;
            continue;
        }

        Record record(firstName, lastName, date, emailString);
        contacts.append(record);
    }
    endResetModel();
    return true;
}

bool ContactModel::saveData(const QString &fileName, bool overwrite)
{
    qDebug() << "Save file:" << fileName;
    if (QFile::exists(fileName) && !overwrite) {
        qDebug() << "File exists and overwrite is forbidden";
        return false;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error when open file for write";
        return false;
    }

    QTextStream stream(&file);
    for (const Record &record : qAsConst(contacts)) {
        QString line = QStringLiteral("%1 %2 %3 %4 ")
                .arg(record.firstName, record.lastName, record.birthday.toString(Qt::ISODate), record.email);
        stream << line << QChar(QChar::LineFeed);
    }

    return true;
}
