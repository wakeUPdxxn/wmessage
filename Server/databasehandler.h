#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFuture>
#include <QThread>
#include <QtConcurrent>
#include <QSemaphore>
#include <server.h>

class Databasehandler : public QObject
{
    Q_OBJECT
public:
    explicit Databasehandler(QObject *parent=nullptr);
    void grabUserData();
    void addUser(const QString &signedUpUserNick,const QString &m_idToken,const QString&userId);
    bool validateUserNick(const QString &nickName);
    friend class Server;
public slots:
    void networkReplyReadyRead();
signals:
    void processed(const QByteArray &response);
private:
    QByteArray response;
    QNetworkAccessManager * m_networkAcessManager;
    QNetworkReply * m_networkReply;
};

