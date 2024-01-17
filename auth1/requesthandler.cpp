#include "requesthandler.h"

RequestHandler::RequestHandler()
{

}

QVariantMap RequestHandler::checkRequest(const QHttpServerRequest &request,const QString type){
    parseRequest(request,type);
    static QRegularExpression mailRex("([0-9A-Za-z]+@+[a-z]+\\.+[A-Za-z]+$)");
    static QRegularExpression passRex("([0-9A-Za-z]+$)");
    if(mailRex.match(m_requestData["email"].toString()).hasMatch() && passRex.match(m_requestData["password"].toString()).hasMatch()) {
        return m_requestData;
    }
    else {
        setErrorString(QString("error"));
        m_requestData["error"]="ERROR TYPE";
        if(!passRex.match(m_requestData["password"].toString()).hasMatch()){
            m_requestData["reason"]="incorrect password";
            return m_requestData;
        }
        if(!mailRex.match(m_requestData["email"].toString()).hasMatch()){
            m_requestData["reason"]="incorrect email";
            return m_requestData;
        }
    }
}

void RequestHandler::parseRequest(const QHttpServerRequest &request,const QString type)
{
    if(type == "logIn"){
        QVariantMap requestBody=QJsonDocument::fromJson(request.body()).object().toVariantMap();
        m_requestData["email"]=requestBody["email"];
        m_requestData["password"]=requestBody["password"];
        m_requestData["returnSecureToken"]=requestBody["returnSecureToken"];
    }
}
