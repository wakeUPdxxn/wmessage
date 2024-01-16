#pragma once
#include <QException>

class Exception:public QException
{
public:
    Exception(const QString &text=" ") throw();
    ~Exception() = default;
    void raise() const override;
    Exception *clone() const override { return new Exception(*this); }
    const char *what() const throw() override;

private:
    QString message;
};
