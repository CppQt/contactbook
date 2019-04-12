#ifndef DATEVALIDATOR_H
#define DATEVALIDATOR_H

#include <QValidator>

class DateValidator : public QValidator
{
    Q_OBJECT
public:
    explicit DateValidator(QObject *parent = nullptr);

    virtual State validate(QString &input, int &pos) const override;
};

#endif // DATEVALIDATOR_H
