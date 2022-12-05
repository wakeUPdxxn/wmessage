#include "server.h"

Server::Server()
{
    auth.setApiKey("AIzaSyArYs6ByVk8DB9QU7eQGenWk_QOZBMLPCw");
    if(this->listen(QHostAddress::Any,3232)){
        qDebug() << "start";
    }
    else {
        qDebug() << "error";
    }
    nextBlockSize=0;
}

void Server::incomingConnection(qintptr socketDescriptor){
    socket=new QTcpSocket;
    addPendingConnection(socket);
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket,&QTcpSocket::readyRead,this,&Server::readyRead);
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnectedEvent()));
    connect(socket,&QTcpSocket::disconnected,socket,&QTcpSocket::deleteLater); //Очистка сокета при получении сигнала об отключении клиента

    SClients[socket->socketDescriptor()]=socket;
    qDebug() << "Client connected" << socket->socketDescriptor();
}

void Server::readyRead(){
    socket=(QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_4);
    if(in.status()==QDataStream::Ok){
        qDebug() << "Message from" << socket->socketDescriptor();
        for( ; ; ){
            if(nextBlockSize==0){
                if(socket->bytesAvailable()<2){
                    break;
                }
                in >> nextBlockSize;
            }
            if(socket->bytesAvailable()<nextBlockSize){
                break;
            }
            struct RESPONSE{
                QString payload;
                QString email;
                QString password;
            }response;
            in >> response.payload;

            if(response.payload=="authentication"){
               in >> response.email >> response.password;
               QString email=response.email;
               QString password=response.password;
               qDebug() << "Authentication required with data" << "email:" << email << "password:" << password;
               auth.signUserIn(email,password);
               sendToClient(auth.result,socket->socketDescriptor());
            }
            if(response.payload=="registration"){
                in >> response.email >> response.password;
                QString email=response.email;
                QString password=response.password;
                qDebug() << "Registration required with data" << "email:" << email << "password:" << password;
                auth.signUserUp(email,password);
                sendToClient(auth.result,socket->socketDescriptor());
            }
            nextBlockSize=0;
            break;
        }
    }
    else {
        qDebug() << "DataStream error";
    }
}

void Server::disconnectedEvent()
{
    QTcpSocket * clientSock = qobject_cast<QTcpSocket *>(sender());
    if (NULL == clientSock) {
        return;
    }
    qDebug() << "Client disconnected" << clientSock->peerAddress();
}

void Server::sendToClient(QString str,quint16 accepter)
{
    Data.clear();
    QDataStream out(&Data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out << quint16(0) << str;
    out.device()->seek(0);
    out << quint16(Data.size()-sizeof(quint16));
    SClients[accepter]->write(Data);
}



