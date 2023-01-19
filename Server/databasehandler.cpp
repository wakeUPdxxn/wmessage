#include "databasehandler.h"

Databasehandler::Databasehandler(QObject *parent,QString &m_idToken)
    : QObject{parent},
      accessedUserIdToken(m_idToken)
{
    m_networkAcessManager=new QNetworkAccessManager(this);
}

void Databasehandler::grabUserData()
{
    QString endPoint="https://messanger-80c21-default-rtdb.europe-west1.firebasedatabase.app/authentication/users.json?auth=" + accessedUserIdToken;
    m_networkReplay = m_networkAcessManager->get(QNetworkRequest(QUrl(endPoint)));
    connect(m_networkReplay,&QNetworkReply::readyRead,this,&Databasehandler::networkReplyReadyRead);
}

void Databasehandler::networkReplyReadyRead()
{
    QByteArray response = m_networkReplay->readAll();
    m_networkReplay ->deleteLater();
    qDebug() << response;
}
