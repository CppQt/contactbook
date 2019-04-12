#include "datevalidator.h"

#include <QDate>

DateValidator::DateValidator(QObject *parent) : QValidator(parent)
{
}

QValidator::State DateValidator::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos)
    auto date = QDate::fromString(input, Qt::DefaultLocaleShortDate);
    return date.isValid() ? QValidator::Acceptable : QValidator::Invalid;
}
