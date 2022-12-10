#include "server.h"
#include <QDebug>
#include <QFile>

Server::Server(QObject *parent)
    : QWebSocketServer{"MyServer",QWebSocketServer::NonSecureMode,parent}
{
    QFile data("C:/Users/DXXN/Documents/Client-Server-messanger/Server/apiKey.txt");
    if(data.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&data);
        auth.setApiKey(in.readLine());
    }
    if(this->listen(QHostAddress::Any,2323)){
        qDebug() << "start";
    }
    else {
        qDebug() << "error";
    }
    socket=NULL;
    connect(this,&Server::newConnection,this, &Server::newClient);
}

void Server::newClient(){

    socket = new QWebSocket;
    socket = this->nextPendingConnection();
    connect(socket,&QWebSocket::textMessageReceived,this,&Server::readyRead);
    connect(socket,&QWebSocket::disconnected,this,&Server::disconnectedEvent);
    connect(socket,&QWebSocket::disconnected,socket,&QWebSocket::deleteLater); //Очистка сокета при получении сигнала об отключении клиента

    SClients[socket->peerAddress()]=socket;
    qDebug() << "Client connected" << socket->peerAddress();
}

void Server::readyRead(const QString &message){
    socket=(QWebSocket*)sender();
    qDebug() << "Message from" << socket->peerAddress() << message;
}

void Server::disconnectedEvent()
{
    QWebSocket *clientSock= qobject_cast<QWebSocket *>(sender());
    if (NULL == clientSock) {
        return;
    }
    qDebug() << "Client disconnected" << clientSock->peerAddress();
}

void Server::sendToClient(const QString &str,const QHostAddress &clientAddress)
{
    Data.clear();
    QDataStream out(&Data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out << quint16(0) << str;
    out.device()->seek(0);
    out << quint16(Data.size()-sizeof(quint16));
    SClients[clientAddress]->sendTextMessage(str);
}



