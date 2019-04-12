#include "contactmodel.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QEventLoop>
#include <QDebug>

#include "asyncfileloader.h"

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
    return m_contacts.size();
}

QVariant ContactModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_contacts.size())
        return QVariant();

    switch (static_cast<Roles>(role)) {
    case Roles::FirstNameRole:
        return m_contacts.at(index.row()).firstName;
    case Roles::LastNameRole:
        return m_contacts.at(index.row()).lastName;
    case Roles::BirthdayRole:
        return m_contacts.at(index.row()).birthday;
    case Roles::EmailRole:
        return m_contacts.at(index.row()).email;
    default:
        return QVariant();
    }
}

bool ContactModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() >= m_contacts.size())
        return false;

    switch (static_cast<Roles>(role)) {
    case Roles::FirstNameRole:
        m_contacts[index.row()].firstName = value.toString();
        break;
    case Roles::LastNameRole:
        m_contacts[index.row()].lastName = value.toString();
        break;
    case Roles::BirthdayRole:
        m_contacts[index.row()].birthday = value.toDate();
        break;
    case Roles::EmailRole:
        m_contacts[index.row()].email = value.toString();
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

    AsyncFileLoader::instance()->setFileName(fileName);
    if (!AsyncFileLoader::instance()->fileExists()) {
        return false;
    }

    QEventLoop loop;
    connect(AsyncFileLoader::instance(), &AsyncFileLoader::loadingFinished, &loop, &QEventLoop::quit);
    connect(this, &ContactModel::newLineNeeded, AsyncFileLoader::instance(), &AsyncFileLoader::loadNextLine, Qt::QueuedConnection);
    connect(this, &ContactModel::stopLoadingNeeded, AsyncFileLoader::instance(), &AsyncFileLoader::stopLoading, Qt::QueuedConnection);
    connect(AsyncFileLoader::instance(), &AsyncFileLoader::lineLoaded, this, &ContactModel::processLine);
    connect(AsyncFileLoader::instance(), &AsyncFileLoader::errorOccured, [&loop](const QString &reason) {
        qDebug() << "Error:" << reason;
        loop.quit();
    });
    AsyncFileLoader::instance()->startLoading();
    if (AsyncFileLoader::instance()->error().isEmpty()) {
        beginResetModel();
        m_contacts.clear();
        emit newLineNeeded();
        loop.exec();
        endResetModel();
        AsyncFileLoader::destroyInstance();
        return true;
    }

    AsyncFileLoader::destroyInstance();
    return false;
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
    for (const Record &record : qAsConst(m_contacts)) {
        QString line = QStringLiteral("%1 %2 %3 %4 ")
                .arg(record.firstName, record.lastName, record.birthday.toString(Qt::ISODate), record.email);
        stream << line << QChar(QChar::LineFeed);
    }

    return true;
}

bool ContactModel::addRowBefore(int row, const QString &firstName, const QString &lastName, const QDate &birthday, const QString &email)
{
    if (row < 0 || row >= m_contacts.size()) {
        return false;
    }

    beginInsertRows({}, row, row);
    Record record(firstName, lastName, birthday, email);
    m_contacts.insert(row, record);
    endInsertRows();
    return true;
}

bool ContactModel::addRowAfter(int row, const QString &firstName, const QString &lastName, const QDate &birthday, const QString &email)
{
    if (row < 0 || row >= m_contacts.size()) {
        return false;
    }
    beginInsertRows({}, row + 1, row + 1);
    Record record(firstName, lastName, birthday, email);
    m_contacts.insert(row + 1, record);
    endInsertRows();
    return true;
}

bool ContactModel::appendRow(const QString &firstName, const QString &lastName, const QDate &birthday, const QString &email)
{
    int row = m_contacts.size();
    beginInsertRows({}, row, row);
    Record record(firstName, lastName, birthday, email);
    m_contacts.append(record);
    endInsertRows();
    return true;
}

bool ContactModel::removeRow(int row)
{
    if (row < 0 || row >= m_contacts.size()) {
        return false;
    }
    beginRemoveRows({}, row, row);
    m_contacts.remove(row);
    endRemoveRows();
    return true;
}

void ContactModel::stopLoading()
{
    emit stopLoadingNeeded();
}

void ContactModel::processLine(const QString &line)
{
#ifdef QT_DEBUG
    qDebug() << "Line:" << line;
#endif // QT_DEBUG
    QStringList parts = line.split(QRegularExpression("\\s+"));
    if (parts.size() < 4) {
        qDebug() << "Not enough fields" << line;
        emit newLineNeeded();
        return;
    }

    QString firstName = parts.at(0);
    QString lastName = parts.at(1);
    QString dateString = parts.at(2);
    QDate date = QDate::fromString(dateString, Qt::ISODate);
    if (!date.isValid()) {
        qDebug() << "Date is invalid" << dateString;
        emit newLineNeeded();
        return;
    }
    QString emailString = parts.at(3);
    QRegularExpression vRegExp(EMAIL_VALIDATOR);
    if (!vRegExp.match(emailString).hasMatch()) {
        qDebug() << "Email is invalid" << emailString;
        emit newLineNeeded();
        return;
    }

    Record record(firstName, lastName, date, emailString);
    m_contacts.append(record);
    emit newLineNeeded();
}
