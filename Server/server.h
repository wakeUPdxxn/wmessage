#pragma once

#include <QObject>
#include <QWebSocket>
#include <QWebSocketServer>
#include <QThread>
#include <QStack>

class Server:public QWebSocketServer
{
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
private:
    QHash<QPair<QString,const QHostAddress>,QWebSocket *> SClients;
    QStack<QHostAddress>findUserCallers;
    QStack<QString>requeiredNicksToFind;
    void sendUsersList(const QList<QVariantMap>& users, const QHostAddress &requestSenderAddr);

public slots:
    void newClient();
    void requestRecived(const QByteArray &data);
    void disconnectedEvent();
    void sendAuthResultToClient(const QByteArray &response, const QHostAddress &clientAddress);
    void makeUsersList(const QByteArray& response);
    void setUserId(const QString&UID,const QHostAddress &userAddress);
    void sendMessageToClient(const QString&idSender,const QString&idReceiver,const QString& message);
};

