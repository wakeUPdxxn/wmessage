#include "login.h"
#include "qjsonobject.h"
#include "ui_login.h"
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    setFixedSize(496,460);
    setWindowTitle("Authentication");
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->error->setVisible(false);
    m_NetAccessManager=new QNetworkAccessManager();
}

Login::~Login()
{
    if(m_NetReply!=nullptr){
        m_NetReply->deleteLater();
    }
    if(m_NetAccessManager!=nullptr){
        m_NetAccessManager->deleteLater();
    }
    delete ui;
}

void Login::sendPost(const QJsonDocument &requestData)
{
    QNetworkRequest newRequest( (QUrl( api )) );
    newRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    m_NetReply = m_NetAccessManager->post(newRequest,requestData.toJson());
    connect(m_NetReply,&QNetworkReply::readyRead,this,&Login::parseResponse);
}

void Login::makeRequest(const QString &email, const QString &password)
{
    QVariantMap payload;
    payload["email"]=email;
    payload["password"]=password;
    payload["returnSecureToken"]=true;

    QJsonDocument jsonPayload=QJsonDocument::fromVariant(payload);
    sendPost(jsonPayload);
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
    QString email=ui->email->text();
    QString password=ui->password->text();
    if(email.isEmpty()||password.isEmpty()){
        if(email.isEmpty()){
            //QMessageBox::information(this,"Error","Поле email не заполнено");
        }
        if(password.isEmpty()){
            //QMessageBox::information(this,"Error","Поле password не заполнено");
        }
    }
    else{
        makeRequest(email,password);
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

void Login::parseResponse()
{
    QByteArray bytes = m_NetReply->readAll();
    QJsonDocument response = QJsonDocument::fromJson(bytes);

    if(response.object().contains("error")){
        QString reason=response.object()["reason"].toString();
        if(reason=="incorrect email"){
            ui->error->setText("incorrect email");
            ui->error->setVisible(true);
        }
        else if(reason=="incorrect password"){
            ui->error->setText("incorrect password");
            ui->error->setVisible(true);
        }
    }
    else if (response.object().contains("refreshToken")) {
        QString refreshToken = response.object()["refreshToken"].toString();

        QFile userData("./userData.json");
        userData.open(QIODevice::ReadOnly);
        QJsonObject obj=QJsonDocument::fromJson(userData.readAll()).object();
        userData.close();

        obj["refreshToken"]=refreshToken;
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

