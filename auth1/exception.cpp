#include "exception.h"

Exception::Exception(const QString &text) throw()
           :message(text){}

void Exception::raise() const
{
    throw *this;
}

const char *Exception::what() const throw()
{
    return this->message.toStdString().c_str();
}
