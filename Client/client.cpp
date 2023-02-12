#include "client.h"
#include "login.h"
#include "qdialogbuttonbox.h"
#include "qformlayout.h"
#include "ui_client.h"

Client::Client(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::Client)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    socket=new QWebSocket;
    socket->open(QUrl("ws://localhost:2323"));
    if(socket->state()!=QAbstractSocket::ConnectingState){
       QMessageBox::critical(this,"Error","Connection to server faild");
       exit(1);
    }
    connect(socket,SIGNAL(textMessageReceived(QString)),this,SLOT(messageReceived(QString)));
    connect(socket,SIGNAL(binaryMessageReceived(QByteArray)),this,SLOT(binaryMessageReceived(QByteArray)));
    connect(socket,&QWebSocket::disconnected,socket,&QWebSocket::deleteLater);

    if(!isAuthorized()){
        showSignInForm();
    }
    else{
        this->show();
    }
}

Client::~Client()
{
    delete ui;
}

bool Client::isAuthorized()
{
    QFile userData("C:/Qt/projects/Client-Server-messanger/Client/userData.json");
    userData.open(QIODevice::ReadOnly);
    QString data;
    data=userData.readAll();
    userData.close();
    QJsonDocument doc=QJsonDocument::fromJson(data.toUtf8());
    QJsonObject obj=doc.object();
    if(obj["authorized"].toBool()==false){
       return false;
    }
    else{
       return true;
    }
}

void Client::swithAuthorizedState(){
    QFile userData("C:/Qt/projects/Client-Server-messanger/Client/userData.json");
    userData.open(QIODevice::ReadOnly);
    QString data;
    data=userData.readAll();
    userData.close();
    QJsonDocument doc=QJsonDocument::fromJson(data.toUtf8());
    QJsonObject obj=doc.object();
    if(obj["authorized"].toBool()==false){
        obj["authorized"]=true;
    }
    else{
        obj["authorized"]=false;
    }
    userData.open(QIODevice::WriteOnly);
    userData.write(QJsonDocument(obj).toJson());
}


void Client::showSignInForm() {
   Login *log=new Login(nullptr,socket);
   log->setWindowTitle("Authentication");
   log->setWindowFlag(Qt::WindowStaysOnTopHint);
   log->show();
   connect(log,&Login::signedIn,this,&Client::show);
   connect(log,&Login::signedIn,this,&Client::swithAuthorizedState);
   connect(log,&Login::signUpRequired,this,&Client::showSignUpForm);
}

void Client::showSignUpForm() {
    Registration *reg = new Registration(nullptr,socket);
    reg->setWindowTitle("Registration");
    reg->setWindowFlag(Qt::WindowStaysOnTopHint);
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

void Client::binaryMessageReceived(const QByteArray &msg)
{
    QDataStream data(msg);
    QString message;
    data >> message;
    qDebug() << message;
}

void Client::sendMessage(const QString &message)
{
    socket->sendTextMessage(message);
}



