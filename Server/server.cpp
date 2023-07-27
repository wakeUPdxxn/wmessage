#include "server.h"
#include "qforeach.h"
#include <QDebug>
#include <QFile>
#include <authentication.h>
#include <databasehandler.h>

Server::Server(QObject *parent)
    : QWebSocketServer{"MyServer",QWebSocketServer::NonSecureMode,parent}
{
    QFile data("./apiKey.txt");
    if(data.open(QIODevice::ReadOnly)){
        QTextStream in(&data);
        Authentication::setApiKey(QString(in.readLine()));
        data.close();
    }
    else{
        qDebug() << "Unable to read apiKey file";
    }
    if(this->listen(QHostAddress::Any,2323)){
        qDebug() << "Server started";
    }
    else {
        qDebug() << "Error occurred while starting";
    }
    connect(this,&Server::newConnection,this, &Server::newClient);
}

Server::~Server()
{

}

void Server::newClient(){

    QWebSocket *socket;
    socket = this->nextPendingConnection();
    connect(socket,&QWebSocket::binaryMessageReceived,this,&Server::requestRecived);
    connect(socket,&QWebSocket::disconnected,this,&Server::disconnectedEvent);
    connect(socket,&QWebSocket::disconnected,socket,&QWebSocket::deleteLater); //Очистка сокета при получении сигнала об отключении клиента

    SClients[qMakePair("",socket->peerAddress())]=socket;
    qDebug() << "Client connected" << socket->peerAddress();
}


void Server::requestRecived(const QByteArray &data)
{
    QWebSocket *socket;
    socket=(QWebSocket*)sender();
    QDataStream request(data);
    QString payload;
    request >> payload;
    if(payload=="comeBack"){
        QString UID;
        request >> UID;
        setUserId(UID,socket->peerAddress());
    }
    if(payload=="authentication"){
        Authentication *auth=new Authentication(this);
        auth->setClientToResponseAddress(socket->peerAddress());
        connect(auth,&Authentication::readyToResponse,this,&Server::sendAuthResultToClient);
        connect(auth,&Authentication::userIdCaptured,this,&Server::setUserId);
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
        Authentication *auth=new Authentication(this);
        auth->setClientToResponseAddress(socket->peerAddress());
        connect(auth,&Authentication::readyToResponse,this,&Server::sendAuthResultToClient);
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
        auth->signUserUp(email,password,nickname);
    }
    else if(payload=="findUser"){
        QString nickName;
        request >> nickName;
        findUserCallers.push(socket->peerAddress());
        requeiredNicksToFind.push(nickName);
        Databasehandler *db=new Databasehandler(this);
        connect(db,&Databasehandler::processed,this,&Server::makeUsersList);
        db->grabUserData();
    }
    else if(payload=="message"){
        QString receiverUID;
        QString senderUID;
        QString message;
        request >> receiverUID;
        request >> senderUID;
        request >> message;
        sendMessageToClient(senderUID,receiverUID,message);
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

void Server::sendUsersList(const QList<QVariantMap>& users, const QHostAddress &requestSenderAddr)
{
    QByteArray response;
    QDataStream data(&response,QIODevice::WriteOnly);
    data << QString("UsersList");
    data << users;
    foreach(const auto &key,SClients.keys()){
        if(key.second==requestSenderAddr){
            SClients[{key.first,requestSenderAddr}]->sendBinaryMessage(response);
            return;
        }
    }
}

void Server::sendAuthResultToClient(const QByteArray &response, const QHostAddress &clientAddress)
{
    SClients[{"",clientAddress}]->sendBinaryMessage(response);
}

void Server::makeUsersList(const QByteArray& response){
    QList<QVariantMap>users;
    QVariantMap userData;
    QJsonObject json=QJsonDocument::fromJson(response).object();
    foreach(const QString& key, json.keys()) {
          auto user = json[key].toObject();
          if(requeiredNicksToFind.top()<=user.value("nickName").toString() && user.value("nickName").toString().contains(requeiredNicksToFind.top())){
            userData["UID"]=user.value("UID").toString();
            userData["nickName"]=user.value("nickName").toString();
            userData["status"]=user.value("status").toString();
            users.emplaceBack(userData);
          }
    }
    if(!users.isEmpty()){
        sendUsersList(users,findUserCallers.top());
        findUserCallers.pop();
        requeiredNicksToFind.pop();
    }
    else {
        userData["nickName"]=QString("No matching results");
        users.emplaceBack(userData);
        sendUsersList(users,findUserCallers.top());
        findUserCallers.pop();
        requeiredNicksToFind.pop();
    }
}

void Server::setUserId(const QString &UID, const QHostAddress &userAddress)
{
    auto pos = SClients.find(qMakePair("",userAddress));
    auto user=*pos;
    SClients[qMakePair(UID,userAddress)]=user;
    SClients.erase(pos);
}

void Server::sendMessageToClient(const QString &idSender, const QString &idReceiver, const QString &message)
{
    QByteArray bytes;
    QDataStream data(&bytes,QIODevice::WriteOnly);
    data << QString("newMessage");
    data << idSender;
    data << QTime::currentTime().toString().chopped(3)+QString(" ")+message;
    foreach(const auto&key,SClients.keys()){
        if(key.first==idReceiver){
            SClients[{idReceiver,key.second}]->sendBinaryMessage(bytes);
            return;
        }
    }
}

