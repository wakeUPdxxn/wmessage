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

    socket= new QTcpSocket(this);
    if(socket->state()==QTcpSocket::UnconnectedState){
        socket->connectToHost("127.0.0.1",3232);
    }
    if(socket->state()==QTcpSocket::UnconnectedState){
        QMessageBox::critical(this,"Error","Connection to server faild");
        exit(1);
    }
    connect(socket,&QTcpSocket::readyRead,this,&Client::ReadyRead);
    connect(socket,&QTcpSocket::disconnected,socket,&QTcpSocket::deleteLater);
    if(authorized==false){
        login();
    }
    nextBlockSize=0;
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

void Client::ReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_4);
    if(in.status()==QDataStream::Ok){
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
            QString str;
            in >> str;
            ui->textBrowser->append(str);
            nextBlockSize=0;
        }
    }
    else{
         QMessageBox::critical(this,"","Not Ok");
    }
}

void Client::SendToServer(QString str)
{
    Data.clear();
    QDataStream out(&Data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out << quint16(0) << str;
    out.device()->seek(0);
    out << quint16(Data.size()-sizeof(quint16));
    socket->write(Data);
}

void Client::on_lineEdit_returnPressed()
{
    QString str=ui->lineEdit->text();
    SendToServer(str);
}

