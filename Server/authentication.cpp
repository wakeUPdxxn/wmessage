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
    clientToResponseAdress=clientAddress;
}

void Authentication::signUserUp(const QString &email, const QString &password,const QString &nickname)
{
    db->grabUserData();
    QFuture<bool>isUnique= QtConcurrent::run(&Databasehandler::validateUserNick,nickname);
    isUnique.waitForFinished();
    if(!isUnique.result()){
        emit readyToResponse(QString("NickAlreadyExist"),clientToResponseAdress);
        return;
    }
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

void Authentication::setApiKey(const QString &apiKey)
{
    m_apiKey=apiKey;
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
        if(currentOperation=="signIn"){
            emit readyToResponse(QString("SignInError"),clientToResponseAdress);
            qDebug() << "SignIn faild";
        }
        else{
            emit readyToResponse(QString("SignUpError"),clientToResponseAdress);
            qDebug() << "SignUp faild";
        }
    }
    else if(jsonDocument.object().contains("kind")){
        m_idToken = jsonDocument.object().value("idToken").toString();
        if(currentOperation=="signIn"){
            emit readyToResponse(QString("SignInSuccess"),clientToResponseAdress);
            emit userSignedIn();
            qDebug() << "SignIn success";
        }
        else{
            emit readyToResponse(QString("SignUpSuccess"),clientToResponseAdress);
            emit userSignedUp();
            qDebug() << "SignUp success";
        }
    }
}

void Authentication::performAuthenticatedDatabaseCall(){
    db->grabUserData();
}

void Authentication::addNewUserToDatabase(){
    db->addUser(signingUpUserNick,m_idToken);
}

