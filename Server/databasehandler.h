#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class Databasehandler : public QObject
{
    Q_OBJECT
public:
    Databasehandler(QObject *parent,QString &m_idToken);
    void grabUserData();

public slots:
    void networkReplyReadyRead();

private:
    QNetworkAccessManager * m_networkAcessManager;
    QNetworkReply * m_networkReplay;
    QString accessedUserIdToken;

};

#endif // DATABASEHANDLER_H
