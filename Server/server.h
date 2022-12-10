#pragma once

#include <QObject>
#include <QWebSocket>
#include <QWebSocketServer>
#include <authentication.h>

class Server:public QWebSocketServer
{
public:
    explicit Server(QObject *parent = nullptr);
private:
    QWebSocketServer *serv;
    QWebSocket *socket;
    QHash<const QHostAddress,QWebSocket *> SClients;
    QByteArray Data;
    Authentication auth;
    void sendToClient(const QString &str,const QHostAddress &clientAddress);

public slots:
    void newClient();
    void readyRead(const QString &str);
    void disconnectedEvent();
};

