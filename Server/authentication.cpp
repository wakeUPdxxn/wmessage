#include "authentication.h"
#include <QDebug>
#include <QVariantMap>
#include <QNetworkRequest>

Authentication::Authentication(QObject *parent,QWebSocket *socket)
    : QObject{parent},
      clientToResponseSocket(socket)
{
    m_networkAcessManager=new QNetworkAccessManager(this);
    connect(this,SIGNAL(userSignedIn()),this,SLOT(performAuthenticatedDatabaseCall()));
}

Authentication::~Authentication()
{
    m_networkAcessManager->deleteLater();
}

void Authentication::setApiKey(const QString &apiKey)
{
    m_apiKey=apiKey;
}

void Authentication::signUserUp(const QString &email, const QString &password)
{
    QString signUpEndpoint = "https://identitytoolkit.googleapis.com/v1/accounts:signUp?key="+m_apiKey;
    QVariantMap payload;
    payload["email"]=email;
    payload["password"]=password;
    payload["returnSecureToken"]=true;

    QJsonDocument jsonPayload=QJsonDocument::fromVariant(payload);
    sendPost(signUpEndpoint,jsonPayload);
}

void Authentication::signUserIn(const QString &email, const QString &password)
{
    QString signInEndpoint = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key="+m_apiKey;
    QVariantMap payload;
    payload["email"]=email;
    payload["password"]=password;
    payload["returnSecureToken"]=true;

    QJsonDocument jsonPayload=QJsonDocument::fromVariant(payload);
    sendPost(signInEndpoint,jsonPayload);
}

void Authentication::networkReplyReadyRead()
{
    QByteArray response = m_networkReplay->readAll();
    m_networkReplay ->deleteLater();
    qDebug() << response;
    parseResponse(response);
}

void Authentication::sendPost(const QString &url, const QJsonDocument &payload)
{
    QNetworkRequest newRequest( (QUrl( url )) );
    newRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    m_networkReplay = m_networkAcessManager->post(newRequest,payload.toJson());
    connect(m_networkReplay,&QNetworkReply::readyRead,this,&Authentication::networkReplyReadyRead);
}

void Authentication::parseResponse(const QByteArray &response)
{
    QJsonDocument jsonDocument =QJsonDocument::fromJson(response);
    if(jsonDocument.object().contains("error")){
        emit readyToResponse(QString("error"),clientToResponseSocket->peerAddress());
        qDebug() << "error";
    }
    else if(jsonDocument.object().contains("kind")){
        QString idToken = jsonDocument.object().value("idToken").toString();
        m_idToken=idToken;
        emit readyToResponse(QString("success"),clientToResponseSocket->peerAddress());
        emit userSignedIn();
        qDebug() << "success";
    }
}

void Authentication::performAuthenticatedDatabaseCall(){
    Databasehandler *dbHandle = new Databasehandler(this,m_idToken);
    dbHandle->grabUserData();
}

