#include "registration.h"
#include "qjsonobject.h"
#include "ui_registration.h"
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>

Registration::Registration(QWidget *parent,QWebSocket *sock) :
    QWidget(parent),
    ui(new Ui::Registration),
    socket(sock)
{
    ui->setupUi(this);
    setFixedSize(496,490);
    setWindowTitle("Registration");
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->error->setVisible(false);
}

Registration::~Registration()
{
    delete ui;
}

void Registration::sendData()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out << request.payload;
    out << request.nickname;
    out << request.email;
    out << request.password;

    socket->sendBinaryMessage(data);
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
    QString username=ui->username->text();
    QString email=ui->email->text();
    QString password=ui->password->text();
    if(email.isEmpty()||password.isEmpty() || username.isEmpty()){
        if(email.isEmpty()){
            QMessageBox::information(this,"Error","Email is not filled ");
        }
        if(password.isEmpty()){
            QMessageBox::information(this,"Error","Password is not filled");
        }
        if(username.isEmpty()){
            QMessageBox::information(this,"Error","Username is not filled");
        }
    }
    else{
        connect(socket,SIGNAL(binaryMessageReceived(QByteArray)),this,SLOT(responseReceived(QByteArray)));
        request.payload=payload;
        request.nickname=username;
        request.email=email;
        request.password=password;
        sendData();
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

void Registration::on_username_returnPressed()
{
    on_registrate_released();
}

void Registration::responseReceived(const QByteArray &response)
{
    QDataStream data(response);
    QString result;
    data >> result;
    if(result=="SignUpError"){
        ui->error->setVisible(true);
    }
    else if(result=="SignUpSuccess"){
        QString accesToken;
        QString refreshToken;
        QString UID;
        data >> accesToken;
        data >> refreshToken;
        data >> UID;
        QFile userData("./userData.json");
        userData.open(QIODevice::ReadOnly);
        QJsonObject obj=QJsonDocument::fromJson(userData.readAll()).object();
        userData.close();
        obj["UID"]=UID;
        obj["nickName"]=request.nickname;
        obj["refreshToken"]=refreshToken;
        obj["accessToken"]=accesToken;
        userData.open(QIODevice::WriteOnly);
        userData.write(QJsonDocument(obj).toJson());
        userData.close();
        emit signedUp();
        this->close();
    }
    else if(result=="NickAlreadyExist"){
        ui->error->setText("Nickname is already in use");
        ui->error->setVisible(true);
    }
}

