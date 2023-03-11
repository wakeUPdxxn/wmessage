#pragma once

#include <QObject>
#include <QWebSocket>
#include <QWebSocketServer>
#include <QThread>

class Server:public QWebSocketServer
{
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
private:
    QWebSocket *socket;
    QHash<const QHostAddress,QWebSocket *> SClients;
    void sendMessageToClient(const QString &message,const QHostAddress &clientAddress);

public slots:
    void newClient();
    void textMessageReceived(const QString &message);
    void binaryMessageRecived(const QByteArray &data);
    void disconnectedEvent();
    void sendBinaryToClient(const QString &message,const QHostAddress &clientAddress);
};

