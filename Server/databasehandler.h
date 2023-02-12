#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class Databasehandler : public QObject
{
    Q_OBJECT
public:
    Databasehandler(QObject *parent,QString &m_idToken);
    void grabUserData();
    void addUser();
    void needToAddNewUser(const QString &nickName);

public slots:
    void networkReplyReadyRead();

private:
    bool addNewUser=false;
    QString signedUpUserNick;
    QByteArray response;
    QNetworkAccessManager * m_networkAcessManager;
    QNetworkReply * m_networkReplay;
    QString accessedUserIdToken;

};

