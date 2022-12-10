#include "login.h"
#include "ui_login.h"
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>

Login::Login(QWidget *parent,QWebSocket *sock) :
    QWidget(parent),
    ui(new Ui::Login),
    socket(sock)
{
    ui->setupUi(this);
    setFixedSize(496,469);
    setAttribute(Qt::WA_DeleteOnClose);
}

Login::~Login()
{
    delete ui;
}


void Login::SendLoginPassword()
{

}

void Login::on_registration_clicked()
{
    reg=new Registration(nullptr,socket);
    reg->setWindowTitle("Регистрация");
    reg->show();
}

void Login::on_login_pressed()
{
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    ui->login->setGraphicsEffect(shadow);
}


void Login::on_login_released()
{
    ui->login->setGraphicsEffect(nullptr);
    QString payload="authentication";
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


void Login::on_show_pressed()
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


void Login::on_password_returnPressed()
{
    on_login_released();
}


void Login::on_email_returnPressed()
{
    on_login_released();
}

