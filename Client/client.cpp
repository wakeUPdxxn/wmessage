#include "client.h"
#include "qdialogbuttonbox.h"
#include "qformlayout.h"
#include "ui_client.h"

Client::Client(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::Client)
{
    ui->setupUi(this);
    //setAttribute(Qt::WA_DeleteOnClose);

    socket=new QWebSocket;
    socket->open(QUrl("ws://localhost:2323"));
    if(socket->state()!=QAbstractSocket::ConnectedState){
       //QMessageBox::critical(this,"Error","Connection to server faild");
       //exit(1);
    }
    connect(socket,SIGNAL(textMessageReceived(QString)),this,SLOT(readyRead(QString)));
    connect(socket,&QWebSocket::disconnected,socket,&QWebSocket::deleteLater);
    if(authorized==false){
        login();
    }
}

Client::~Client()
{
    delete ui;
}

void Client::login() {
   log=new Login(nullptr,socket);
   log->setWindowFlag(Qt::WindowStaysOnTopHint);
   log->setWindowTitle("Авторизация");
   log->show();
}


void Client::on_action_triggered()
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
       QMainWindow::close();
   }
}

void Client::readyRead(const QString &message)
{
    ui->textBrowser->append(message);
}

void Client::SendToServer(QString str)
{
    socket->sendTextMessage(str);
}

void Client::on_lineEdit_returnPressed()
{
    QString str=ui->lineEdit->text();
    SendToServer(str);
}

