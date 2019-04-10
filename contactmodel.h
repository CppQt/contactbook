#ifndef CONTACTMODEL_H
#define CONTACTMODEL_H

#include <QAbstractListModel>
#include <QDate>

class Record
{
public:
    Record() {}
    Record(const Record &other) :
        firstName(other.firstName), lastName(other.lastName), birthday(other.birthday), email(other.email) {}
    Record(Record &&other) :
        firstName(std::move(other.firstName)), lastName(std::move(other.lastName)), birthday(std::move(other.birthday)), email(std::move(other.email)) {}
    Record(const QString &firstName, const QString &lastName, const QDate &birthday, const QString &email) :
        firstName(firstName), lastName(lastName), birthday(birthday), email(email) {}
    Record &operator=(const Record &other) {
        firstName = other.firstName;
        lastName = other.lastName;
        birthday = other.birthday;
        email = other.email;
        return *this;
    }
    Record &operator=(Record &&other) {
        firstName = std::move(other.firstName);
        lastName = std::move(other.lastName);
        birthday = std::move(other.birthday);
        email = std::move(other.email);
        return *this;
    }
    QString firstName;
    QString lastName;
    QDate birthday;
    QString email;
};

class ContactModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        UnknownRole = Qt::UserRole,
        FirstNameRole,
        LastNameRole,
        BirthdayRole,
        EmailRole
    };
    Q_ENUM(Roles)

    explicit ContactModel(QObject *parent = nullptr);

    virtual int rowCount(const QModelIndex &parent = {}) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual QHash<int, QByteArray> roleNames() const override;


    Q_INVOKABLE bool loadData(const QString &fileName);
    Q_INVOKABLE bool saveData(const QString &fileName, bool overwrite = false);

private:
    QVector<Record> contacts;
};

#endif // CONTACTMODEL_H
