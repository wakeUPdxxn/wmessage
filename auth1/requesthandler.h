#pragma once
#include <QHttpServerRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

class RequestHandler
{
public:
    RequestHandler();
    QVariantMap checkRequest(const QHttpServerRequest &request,const QString type);
    QString getLastError() { return errorString; }
    mutable QString errorString;
private:
    void setErrorString(const QString &error) const  { errorString = error; }
    void parseRequest(const QHttpServerRequest &request,const QString type);
    QVariantMap m_requestData;
};



