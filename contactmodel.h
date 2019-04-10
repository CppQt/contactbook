#ifndef CONTACTMODEL_H
#define CONTACTMODEL_H

#include <QAbstractListModel>
#include <QDate>

class Record
{
public:
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
        FirstNameRole = Qt::UserRole,
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


    bool loadData(const QString &fileName);
    bool saveData(const QString &fileName);

private:
    QVector<Record> contacts;
};

#endif // CONTACTMODEL_H
