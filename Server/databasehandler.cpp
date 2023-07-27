#include "databasehandler.h"

Databasehandler::Databasehandler(QObject *parent)
    : QObject{parent}
{
    m_networkAcessManager=new QNetworkAccessManager(this);
}

void Databasehandler::grabUserData()
{
    //QString endPoint2 = "https://firebasestorage.googleapis.com/v1beta/projects/messanger-80c21/buckets/icons?auth="+ users_AccessToken;
    QString endPoint="https://messanger-80c21-default-rtdb.europe-west1.firebasedatabase.app/users.json";
    m_networkReply = m_networkAcessManager->get(QNetworkRequest(QUrl(endPoint)));
    connect(m_networkReply,&QNetworkReply::readyRead,this,&Databasehandler::networkReplyReadyRead,Qt::DirectConnection);
}

void Databasehandler::addUser(const QString &signedUpUserNick, const QString &accessedUserToken, const QString &userId)
{
    QString endPoint="https://messanger-80c21-default-rtdb.europe-west1.firebasedatabase.app/users.json?auth=" + accessedUserToken;
    QVariantMap newUser;
    newUser["UID"]=userId;
    newUser["nickName"]=signedUpUserNick;
    newUser["status"]="Online";
    QJsonDocument jsonDocument=QJsonDocument::fromVariant(newUser);

    QNetworkRequest newUserRequest((QUrl(endPoint)));
    newUserRequest.setHeader(QNetworkRequest::ContentTypeHeader,QString("application/json"));
    m_networkAcessManager->post(newUserRequest,jsonDocument.toJson());
}

void Databasehandler::networkReplyReadyRead()
{  
    response = m_networkReply->readAll();
    m_networkReply ->deleteLater();
    emit processed(response);
}

bool Databasehandler::validateUserNick(const QString &nickName){

}
