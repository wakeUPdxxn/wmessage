#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QWebSocket>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <databasehandler.h>

class Authentication : public QObject
{
    Q_OBJECT
public:
    explicit Authentication(QObject *parent=nullptr);
    ~Authentication();
    void setClientToResponseAddress(const QHostAddress &clientAddress);
    void signUserUp(const QString & email,const QString &password,const QString &nickname);
    void signUserIn(const QString & email,const QString &password);
    static void setApiKey(const QString &apiKey);

public slots:
    void networkReplyReadyRead();

private slots:
    void performAuthenticatedDatabaseCall();
    void addNewUserToDatabase();

signals:
    void userSignedIn();
    void userSignedUp();
    void readyToResponse(const QString &result,const QHostAddress &clientAddress);

private:
    static QString m_apiKey;
    QString signingUpUserNick;
    QString currentOperation;
    Databasehandler *db;
    QHostAddress clientToResponseAdress;
    QNetworkAccessManager * m_networkAcessManager;
    QNetworkReply * m_networkReplay;
    QString m_idToken;
    void sendPost(const QString &url,const QJsonDocument &payload);
    void parseResponse(const QByteArray & response); 
};

