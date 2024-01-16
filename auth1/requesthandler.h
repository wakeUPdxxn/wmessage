#pragma once
#include <QHttpServerRequest>

class RequestHandler
{
public:
    RequestHandler();
    bool checkRequest(const QHttpServerRequest &request);
    QString getLastError() { return errorString; }
    mutable QString errorString;
private:
    void setErrorString(const QString &error) const  { errorString = error; }
};


