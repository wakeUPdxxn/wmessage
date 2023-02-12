#include "databasehandler.h"

Databasehandler::Databasehandler(QObject *parent,QString &m_idToken)
    : QObject{parent},
      accessedUserIdToken(m_idToken)
{
    m_networkAcessManager=new QNetworkAccessManager(this);
}

void Databasehandler::grabUserData()
{
    QString endPoint="https://messanger-80c21-default-rtdb.europe-west1.firebasedatabase.app/users.json?auth=" + accessedUserIdToken;
    m_networkReplay = m_networkAcessManager->get(QNetworkRequest(QUrl(endPoint)));
    connect(m_networkReplay,&QNetworkReply::readyRead,this,&Databasehandler::networkReplyReadyRead);
}

void Databasehandler::addUser()
{
    QString endPoint="https://messanger-80c21-default-rtdb.europe-west1.firebasedatabase.app/users.json?auth=" + accessedUserIdToken;
    QJsonDocument jDoc=QJsonDocument::fromJson(response);
    QVariantMap users=jDoc.object().toVariantMap();
    QVariantMap newUser;
    newUser["id"]=users.size()+1;
    newUser["nickName"]=signedUpUserNick;
    QJsonDocument jsonDocument=QJsonDocument::fromVariant(newUser);

    QNetworkRequest newUserRequest((QUrl(endPoint)));
    newUserRequest.setHeader(QNetworkRequest::ContentTypeHeader,QString("application/json"));
    m_networkAcessManager->post(newUserRequest,jsonDocument.toJson());
}

void Databasehandler::networkReplyReadyRead()
{
    response = m_networkReplay->readAll();
    if(addNewUser){
        addUser();
    }
    m_networkReplay ->deleteLater();
}

void Databasehandler::needToAddNewUser(const QString &nickName)
{
    addNewUser=true;
    signedUpUserNick=nickName;
    grabUserData();
}
