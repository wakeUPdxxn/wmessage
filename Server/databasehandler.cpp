#include "databasehandler.h"

QMutex m_mutex;
QWaitCondition replyRead;

Databasehandler::Databasehandler(QObject *parent)
    : QObject{parent}
{
    m_networkAcessManager=new QNetworkAccessManager(this);
}

void Databasehandler::grabUserData()
{
    QString endPoint="https://messanger-80c21-default-rtdb.europe-west1.firebasedatabase.app/users.json";
    m_networkReply = m_networkAcessManager->get(QNetworkRequest(QUrl(endPoint)));
    connect(m_networkReply,&QNetworkReply::readyRead,this,&Databasehandler::networkReplyReadyRead);
}

void Databasehandler::addUser(const QString &signedUpUserNick,const QString &accessedUserIdToken)
{
    QString endPoint="https://messanger-80c21-default-rtdb.europe-west1.firebasedatabase.app/users.json?auth=" + accessedUserIdToken;
    QVariantMap newUser;
    newUser["nickName"]=signedUpUserNick;
    QJsonDocument jsonDocument=QJsonDocument::fromVariant(newUser);

    QNetworkRequest newUserRequest((QUrl(endPoint)));
    newUserRequest.setHeader(QNetworkRequest::ContentTypeHeader,QString("application/json"));
    m_networkAcessManager->post(newUserRequest,jsonDocument.toJson());
}

void Databasehandler::networkReplyReadyRead()
{  
    response = m_networkReply->readAll();
    qDebug() << response;
    m_networkReply ->deleteLater();
    replyRead.notify_one();
}

bool Databasehandler::validateUserNick(const QString &nickName){
    QMutexLocker lk(&m_mutex);
    qDebug() << "Waiting for response";
    replyRead.wait(&m_mutex);
    qDebug() << "finished";
}
