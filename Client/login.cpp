#include "login.h"
#include "qjsonobject.h"
#include "ui_login.h"
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>

Login::Login(QWidget *parent,QWebSocket *sock) :
    QWidget(parent),
    ui(new Ui::Login),
    socket(sock)
{
    ui->setupUi(this);
    setFixedSize(496,460);
    setWindowTitle("Authentication");
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->error->setVisible(false);
}

Login::~Login()
{
    delete ui;
}

void Login::sendLoginPassword()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    request.payload="authentication";
    out << request.payload;
    out << request.email;
    out << request.password;
    socket->sendBinaryMessage(data);
}

void Login::on_registration_clicked()
{
    emit signUpRequired();
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
        connect(socket,SIGNAL(binaryMessageReceived(QByteArray)),this,SLOT(responseReceived(QByteArray)));
        request.payload=payload;
        request.email=email;
        request.password=password;
        sendLoginPassword();
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

void Login::responseReceived(const QByteArray &response)
{
    QDataStream data(response);
    QString result;
    data >> result;
    if(result=="SignInError"){
        ui->error->setVisible(true);
    }
    else if(result=="SignInSuccess"){
        QString UID;
        QString newAccesToken;
        data >> UID;
        data >> newAccesToken;
        QFile userData("./userData.json");
        userData.open(QIODevice::ReadOnly);
        QJsonObject obj=QJsonDocument::fromJson(userData.readAll()).object();
        userData.close();
        obj["accessToken"]=newAccesToken;
        obj["UID"]=UID;
        userData.open(QIODevice::WriteOnly);
        userData.write(QJsonDocument(obj).toJson());
        userData.close();
        emit signedIn();
        if(ui->keepIn->checkState()==Qt::Checked){
            emit keepIn();
        }
        this->close();
    }
}

void Login::on_email_textChanged(const QString &arg1)
{
    if(ui->error->isVisible()){
        ui->error->setVisible(false);
    }
}

void Login::on_password_textChanged(const QString &arg1)
{
    if(ui->error->isVisible()){
        ui->error->setVisible(false);
    }
}

