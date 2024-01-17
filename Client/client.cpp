#include "client.h"
#include "qdialogbuttonbox.h"
#include "qformlayout.h"
#include "qgraphicseffect.h"
#include "ui_client.h"
#include "login.h"
#include "registration.h"
#include <QHBoxLayout>

Client::Client(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::Client)
{
    ui->setupUi(this);
    socket=new QWebSocket;
    socket->open(QUrl("ws://localhost:2323"));
    if(socket->state()!=QAbstractSocket::ConnectingState){
       QMessageBox::critical(this,"Error","Connection to server faild");
       exit(1);
    }
    connect(socket,SIGNAL(textMessageReceived(QString)),this,SLOT(messageReceived(QString)));
    connect(socket,SIGNAL(binaryMessageReceived(QByteArray)),this,SLOT(responseReceived(QByteArray)));
    connect(socket,&QWebSocket::disconnected,socket,&QWebSocket::deleteLater);

    parseUserData();
    if(isAuthorized) {
        connect(socket,&QWebSocket::connected,this,&Client::sendCallToServer);
        this->show();
    }
    else {
        showSignInForm();
    }
    ui->search->setPlaceholderText("Explore chats");
    ui->message->setVisible(false);
}

void Client::parseUserData(){
    QFile userData("./userData.json");
    userData.open(QIODevice::ReadOnly);
    QJsonObject obj=QJsonDocument::fromJson(userData.readAll()).object();
    userData.close();
    //selfNickName=obj["nickName"].toString();
    //selfUID=obj["UID"].toString();
    isAuthorized = obj["authorized"].toBool();
}

void Client::sendCallToServer()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out << QString("comeBack");
    out << selfUID;
    socket->sendBinaryMessage(data);
}

Client::~Client()
{
    delete ui;
}

void Client::swithAuthorizedState(){
    QFile userData("./userData.json");
    userData.open(QIODevice::ReadOnly);
    QJsonObject obj=QJsonDocument::fromJson(userData.readAll()).object();
    userData.close();
    if(obj["authorized"].toBool()==false){
        obj["authorized"]=true;
    }
    else{
        obj["authorized"]=false;
    }
    userData.open(QIODevice::WriteOnly);
    userData.write(QJsonDocument(obj).toJson());
    userData.close();
}


void Client::showSignInForm() {
   Login *log=new Login();
   log->show();
   connect(log,&Login::signedIn,this,&Client::show);
   connect(log,&Login::keepIn,this,&Client::swithAuthorizedState);
   connect(log,&Login::signUpRequired,this,&Client::showSignUpForm);
}

void Client::showSignUpForm() {
    Registration *reg= new Registration(nullptr,socket);
    reg->show();
}

/*void Client::on_action_triggered()
{
   QDialog logout(this);
   logout.setWindowTitle("Выход");
   QLabel *le1= new QLabel(&logout);
   QLabel *le2= new QLabel(&logout);
   le1->setText("Вы действительно хотите выйти?");
   le2->setText("При входе придется повторно ввести данные");

   QDialogButtonBox *button =new QDialogButtonBox(&logout);
   button->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

   connect(button,&QDialogButtonBox::accepted,&logout,&QDialog::accept);
   connect(button,&QDialogButtonBox::rejected,&logout,&QDialog::reject);

   QFormLayout *layout =new QFormLayout();
   layout->addRow(le1);
   layout->addRow(le2);
   layout->addWidget(button);

   logout.setLayout(layout);

   if(logout.exec()==QDialog::Accepted){
       QMessageBox msg;
       msg.setStyleSheet("background-color: #22222e");
       msg.information(this,"Messanger","До встречи,пользователь");
       swithAuthorizedState();
       QMainWindow::close();
   }
}*/

void Client::messageReceived(const QString &message)
{
    //ui->textBrowser->append(message);
}

void Client::responseReceived(const QByteArray &response)
{

    QDataStream data(response);
    QString payload;
    data >> payload;
    if(payload=="UsersList"){
        ui->dialogsList->clear();
        data >> usersInSearch;
        foreach(const auto &user,usersInSearch){
            if(user["nickName"]!=selfNickName){
                QListWidgetItem *item = new QListWidgetItem;
                item->setForeground(QBrush(QColor(252, 238, 255)));
                item->setFont(QFont("Montserrat Black"));
                item->setText(user["nickName"].toString());
                if(item->text()=="No matching results"){
                    item->setFlags(item->flags() & Qt::ItemIsSelectable);
                }
                ui->dialogsList->addItem(item);
            }
            else {
                QListWidgetItem *item = new QListWidgetItem;
                item->setForeground(QBrush(QColor(252, 238, 255)));
                item->setFont(QFont("Montserrat Black"));
                item->setText("No matching results");
                item->setFlags(item->flags() & Qt::ItemIsSelectable);
                ui->dialogsList->addItem(item);
            }
        }
    }
    else if(payload=="newMessage"){
        QString idSender;
        QString message;
        data >> idSender;
        data >> message;
        QListWidgetItem *item = new QListWidgetItem;
        item->setForeground(QBrush(QColor(252, 238, 255)));
        item->setBackground(QBrush(QColor(11, 117, 255)));
        item->setTextAlignment(Qt::AlignLeft);
        item->setFont(QFont("Montserrat Black",12));
        item->setText(message);
        ui->currentChatList->setBaseSize(QSize(372,416));
        item->setSizeHint(QSize(20,20));
        ui->currentChatList->addItem(item);
    }
}

void Client::on_me_pressed()
{
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(3);
    shadow->setBlurRadius(2);
    ui->me->setGraphicsEffect(shadow);
    ui->me->setStyleSheet("border:2px;"
                          "solid rgb(190, 190, 190);"
                          "border-radius: 6px;");
}


void Client::on_me_released()
{
     ui->me->setGraphicsEffect(nullptr);
     ui->me->setStyleSheet("border:2px solid rgb(99, 99, 99);"
                           "border-radius: 6px;");
}


void Client::on_search_textChanged(const QString &userNick)
{
    if(userNick>chatSearchData){
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        QString payload="findUser";
        out << payload;
        out << userNick;
        socket->sendBinaryMessage(data);
        chatSearchData=userNick;
    }
    if(userNick<chatSearchData){
        ui->search->clear();
    }
    if(userNick.isEmpty()){
        chatSearchData.clear();
        ui->dialogsList->clear();
    }
}

void Client::on_search_editingFinished()
{
    chatSearchData.clear();
}


void Client::on_dialogsList_itemDoubleClicked(QListWidgetItem *item)
{
    ui->userNick->setText(item->text());
    chatSearchData.clear();
    ui->search->clear();
    ui->dialogsList->clear();
    if(ui->noSelectedChatsInfo!=nullptr){
        ui->noSelectedChatsInfo->deleteLater();
    }
    if(ui->message->isHidden()){
        ui->message->setVisible(true);
    }
    QList<QVariantMap>::iterator pos=std::find_if(usersInSearch.begin(),usersInSearch.end(),[&](const QVariantMap&user){
        return user["nickName"].toString()==item->text();
    });
    auto user=*pos;
    currentChat.UID=user["UID"].toString();
    currentChat.nickName=user["nickName"].toString();
    currentChat.status=user["status"].toString();
    ui->status->setText(currentChat.status);
}


void Client::on_message_returnPressed()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString payload="message";
    out << payload;
    out << currentChat.UID;
    out << selfUID;
    out << ui->message->text();
    socket->sendBinaryMessage(data);
    QListWidgetItem *item = new QListWidgetItem;
    item->setForeground(QBrush(QColor(252, 238, 255)));
    item->setBackground(QBrush(QColor(255, 100, 23)));
    item->setTextAlignment(Qt::AlignRight);
    item->setFont(QFont("Montserrat Black",12));
    item->setText(QTime::currentTime().toString().chopped(3)+QString(" ")+ui->message->text());
    ui->currentChatList->addItem(item);
}

