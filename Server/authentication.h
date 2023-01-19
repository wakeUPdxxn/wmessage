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
    explicit Authentication(QObject *parent = nullptr,QWebSocket *clientSocket=nullptr);
    ~Authentication();
    void setApiKey(const QString &apiKey);
    void signUserUp(const QString & email,const QString &password);
    void signUserIn(const QString & email,const QString &password);

public slots:
    void networkReplyReadyRead();
    void performAuthenticatedDatabaseCall();

signals:
    void userSignedIn();
    void userRegistarted();
    void readyToResponse(const QString &result,const QHostAddress &clientAddress);

private:
    QString m_apiKey;
    QWebSocket *clientToResponseSocket;
    QNetworkAccessManager * m_networkAcessManager;
    QNetworkReply * m_networkReplay;
    QString m_idToken;
    void sendPost(const QString &url,const QJsonDocument &payload);
    void parseResponse(const QByteArray & response); 
};

