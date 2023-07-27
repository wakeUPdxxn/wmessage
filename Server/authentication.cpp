#include "authentication.h"
#include <QDebug>
#include <QVariantMap>
#include <QNetworkRequest>

QString Authentication::m_apiKey="";

Authentication::Authentication(QObject *parent)
    : QObject{parent}
{
    m_networkAcessManager=new QNetworkAccessManager(this);
    db =new Databasehandler(this);
    connect(this,SIGNAL(userSignedIn()),this,SLOT(performAuthenticatedDatabaseCall()));
    connect(this,SIGNAL(userSignedUp()),this,SLOT(addNewUserToDatabase()));
}

Authentication::~Authentication()
{
    m_networkAcessManager->deleteLater();
    m_networkReplay->deleteLater();
}

void Authentication::setClientToResponseAddress(const QHostAddress &clientAddress)
{
    clientToResponseAddress=clientAddress;
}

void Authentication::signUserUp(const QString &email, const QString &password,const QString &nickname)
{
//    db->grabUserData();
//    QFuture<bool>isUnique= QtConcurrent::run([this, &nickname]()->bool{
//        return db->validateUserNick(nickname);
//    });
//    isUnique.waitForFinished();
//    if(!isUnique.result()){
//        emit readyToResponse(QString("NickAlreadyExist"),clientToResponseAddress);
//        return;
//    }
    processingUserNick=nickname;
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

void Authentication::setApiKey(const QString &apiKey)
{
    m_apiKey=apiKey;
}

void Authentication::networkReplyReadyRead()
{
    QByteArray response = m_networkReplay->readAll();
    m_networkReplay ->deleteLater();
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
    QByteArray result;
    QDataStream data(&result,QIODevice::WriteOnly);
    if(jsonDocument.object().contains("error")){
        if(currentOperation=="signIn"){
            data << QString("SignInError");
            emit readyToResponse(result,clientToResponseAddress);
        }
        else{  
            data << QString("SignUpError");
            emit readyToResponse(result,clientToResponseAddress);
        }
    }
    else if(jsonDocument.object().contains("kind")){
        users_AccessToken = jsonDocument.object().value("idToken").toString(); 
        if(currentOperation=="signIn"){
            processingUserId = jsonDocument.object().value("localId").toString();
            data << QString("SignInSuccess");
            data << processingUserId;
            data << users_AccessToken;
            emit readyToResponse(result,clientToResponseAddress);
            emit userIdCaptured(processingUserId,clientToResponseAddress);
            emit userSignedIn();
        }
        else{
            user_RefreshToken = jsonDocument.object().value("refreshToken").toString();
            processingUserId = jsonDocument.object().value("localId").toString();
            data << QString("SignUpSuccess");
            data << user_RefreshToken;
            data << users_AccessToken;
            data << processingUserId;
            emit readyToResponse(result,clientToResponseAddress);
            emit userSignedUp();
        }
    }
}

void Authentication::performAuthenticatedDatabaseCall(){
    db->grabUserData();
}

void Authentication::addNewUserToDatabase(){
    db->addUser(processingUserNick,users_AccessToken,processingUserId);
}

