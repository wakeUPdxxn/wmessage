#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <authentication.h>
#include <thread>

class Server:public QTcpServer
{
    Q_OBJECT
public:
    Server();
    QTcpSocket *socket;

private:
    QMap<int,QTcpSocket *> SClients;
    QByteArray Data;
    quint16 nextBlockSize;
    Authentication auth;
    void sendToClient(const QString &str,const quint16 &clientID);

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void readyRead();
    void disconnectedEvent();
};

#endif // SERVER_H
