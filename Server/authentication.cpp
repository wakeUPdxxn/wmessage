#include "authentication.h"
#include <QDebug>
#include <QVariantMap>
#include <QNetworkRequest>

Authentication::Authentication(QObject *parent)
    : QObject{parent}
{
    m_networkAcessManager=new QNetworkAccessManager(this);
    connect(this,SIGNAL(userSignedIn()),this,SLOT(performAuthenticatedDatabaseCall()));
    connect(this,SIGNAL(userSignedUp()),this,SLOT(addNewUserToDatabase()));
}

Authentication::~Authentication()
{
    m_networkAcessManager->deleteLater();
}

void Authentication::setApiKey(const QString &apiKey)
{
    m_apiKey=apiKey;
}

void Authentication::setClientToResponseAddress(const QHostAddress &clientAddress)
{
    clientToResponseAdress=clientAddress;
}

void Authentication::signUserUp(const QString &email, const QString &password,const QString &nickname)
{
    signingUpUserNick=nickname;
    currentOperation="signUp";
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
    currentOperation="signIn";
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
        emit readyToResponse(QString("error"),clientToResponseAdress);
        if(currentOperation=="signIn"){
            qDebug() << "SignIn faild";
        }
        else{
            qDebug() << "SignUp faild";
        }
    }
    else if(jsonDocument.object().contains("kind")){
        QString idToken = jsonDocument.object().value("idToken").toString();
        m_idToken=idToken;
        emit readyToResponse(QString("success"),clientToResponseAdress);
        if(currentOperation=="signIn"){
            emit userSignedIn();
            qDebug() << "SignIn success";
        }
        else{
            emit userSignedUp();
            qDebug() << "SignUp success";
        }
    }
}

void Authentication::performAuthenticatedDatabaseCall(){
    Databasehandler *dbHandler = new Databasehandler(this,m_idToken);
    dbHandler->grabUserData();
}

void Authentication::addNewUserToDatabase(){
    Databasehandler *dbHandler =new Databasehandler(this,m_idToken);
    dbHandler->needToAddNewUser(signingUpUserNick);
}

