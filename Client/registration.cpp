#include "registration.h"
#include "ui_registration.h"
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>

Registration::Registration(QWidget *parent,QWebSocket *sock) :
    QWidget(parent),
    ui(new Ui::Registration),
    socket(sock)
{
    ui->setupUi(this);
    setFixedSize(496,469);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlag(Qt::WindowStaysOnTopHint);
}

Registration::~Registration()
{
    delete ui;
}

void Registration::SendLoginPassword()
{
    Data.clear();
    QDataStream out(&Data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_3);
    out << quint16(0) << response.payload << response.email << response.password;
}

void Registration::on_registrate_pressed()
{
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    ui->registrate->setGraphicsEffect(shadow);
}


void Registration::on_registrate_released()
{
    ui->registrate->setGraphicsEffect(nullptr);
    QString payload="registration";
    QString email=ui->email->text();
    QString password=ui->password->text();
    if(email.isEmpty()||password.isEmpty()){
        if(email.isEmpty()){
            QMessageBox::information(this,"Error","Поле email не заполнено");
        }
        if(password.isEmpty()){
            QMessageBox::information(this,"Error","Поле password не заполнено");
        }
    }
    else{
        response.payload=payload;
        response.email=email;
        response.password=password;
        SendLoginPassword();
    }
}


void Registration::on_show_pressed()
{
    static int i=1;
    ui->password->setEchoMode(QLineEdit::Normal);
    ui->show->setText("Hide");
    ui->show->setStyleSheet("border:2px solid rgb(190, 190, 190);"
                            "color:rgb(252, 238, 255);"
                            "border-radius:5px;");
    if(i%2==0){
        ui->password->setEchoMode(QLineEdit::Password);
        ui->show->setText("Show");
        ui->show->setStyleSheet("background-color: #22222e;"
               "color:rgb(252, 238, 255);"
                "border:2px solid rgb(99, 99, 99);"
                "border-radius: 6px;");
    }
    i++;
}


void Registration::on_email_returnPressed()
{
    on_registrate_released();
}


void Registration::on_password_returnPressed()
{
    on_registrate_released();
}

