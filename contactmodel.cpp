#include "contactmodel.h"

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
    //
}

bool ContactModel::saveData(const QString &fileName)
{
    //
}
