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
#include <QWaitCondition>
#include <QMutex>
#include <QMutexLocker>

class Databasehandler : public QObject
{
    Q_OBJECT
public:
    Databasehandler(QObject *parent);
    void grabUserData();
    void addUser(const QString &signedUpUserNick,const QString &m_idToken);
    static bool validateUserNick(const QString &nickName);

public slots:
    void networkReplyReadyRead();

private:
    QByteArray response;
    QNetworkAccessManager * m_networkAcessManager;
    QNetworkReply * m_networkReply;
};

