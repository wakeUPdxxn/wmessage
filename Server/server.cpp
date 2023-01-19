#include "server.h"
#include <QDebug>
#include <QFile>
#include <authentication.h>

Server::Server(QObject *parent)
    : QWebSocketServer{"MyServer",QWebSocketServer::NonSecureMode,parent}
{
    QFile data("C:/Users/DXXN/Documents/Client-Server-messanger/Server/apiKey.txt");
    if(data.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&data);
        apiKey=in.readLine();
        data.close();
    }
    else{
        qDebug() << "unable to read apiKey file";
    }
    if(this->listen(QHostAddress::Any,2323)){
        qDebug() << "start";
    }
    else {
        qDebug() << "error";
    }
    connect(this,&Server::newConnection,this, &Server::newClient);
}

void Server::newClient(){

    socket = new QWebSocket;
    socket = this->nextPendingConnection();
    connect(socket,&QWebSocket::textMessageReceived,this,&Server::textMessageReceived);
    connect(socket,&QWebSocket::binaryMessageReceived,this,&Server::binaryMessageRecived);
    connect(socket,&QWebSocket::disconnected,this,&Server::disconnectedEvent);
    connect(socket,&QWebSocket::disconnected,socket,&QWebSocket::deleteLater); //Очистка сокета при получении сигнала об отключении клиента


    SClients[socket->peerAddress()]=socket;
    qDebug() << "Client connected" << socket->peerAddress();
}

void Server::textMessageReceived(const QString &message){
    socket=(QWebSocket*)sender();
    qDebug() << "Message from" << socket->peerAddress() << message;
}

void Server::binaryMessageRecived(const QByteArray &data)
{
    socket=(QWebSocket*)sender();
    QDataStream request(data);
    QString payload;
    request >> payload;
    if(payload=="authentication"){
        Authentication *auth=new Authentication(this,socket);
        auth->setApiKey(apiKey);
        connect(auth,&Authentication::readyToResponse,this,&Server::sendBinaryToClient);

        QString email;
        QString password;
        request >> email;
        request >> password;
        qDebug() << "authentication required from" << "from: " << socket->peerAddress()
                 << "with data {"
                 << "email: " << email
                 << "password:" << password
                 << "}";
        auth->signUserIn(email,password);
    }
    else if(payload=="registration"){
        Authentication *auth=new Authentication(this,socket);
        auth->setApiKey(apiKey);
        connect(auth,&Authentication::readyToResponse,this,&Server::sendBinaryToClient);
        QString nickname;
        QString email;
        QString password;
        request >> nickname;
        request >> email;
        request >> password;
        qDebug() << "regestration required from:" << socket->peerAddress()
                 << "with data {"
                 << "nickname" << nickname
                 << "email: " << email
                 << "password:" << password
                 << "}";
        auth->signUserUp(email,password);
    }
}

void Server::disconnectedEvent()
{
    QWebSocket *clientSock= qobject_cast<QWebSocket *>(sender());
    if (clientSock==NULL) {
        return;
    }
    qDebug() << "Client disconnected" << clientSock->peerAddress();
}

void Server::sendMessageToClient(const QString &message,const QHostAddress &clientAddress)
{
    SClients[clientAddress]->sendTextMessage(message);
}

void Server::sendBinaryToClient(const QString &message, const QHostAddress &clientAddress)
{
    QByteArray msg;
    QDataStream data(&msg,QIODevice::WriteOnly);
    data << message;
    SClients[clientAddress]->sendBinaryMessage(msg);
}



