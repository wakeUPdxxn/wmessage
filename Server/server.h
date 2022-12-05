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
    std::thread *thread;
    Authentication auth;
    void sendToClient(QString str,quint16 accepter);

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void readyRead();
    void disconnectedEvent();
};

#endif // SERVER_H
