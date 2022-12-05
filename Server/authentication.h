#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

class Authentication : public QObject
{
    Q_OBJECT
public:
    explicit Authentication(QObject *parent = nullptr);
    ~Authentication();
    void setApiKey(const QString &apiKey);
    void signUserUp(const QString & email,const QString &password);
    void signUserIn(const QString & email,const QString &password);
    QString result;

public slots:
    void networkReplyReadyRead();

signals:

private:
    void sendPost(const QString &url,const QJsonDocument &payload);
    void parseResponse(const QByteArray & response);
    QString m_apiKey;
    QNetworkAccessManager * m_networkAcessManager;
    QNetworkReply * m_networkReplay;
    QString m_idToken;

};

#endif // AUTHENTICATION_H
