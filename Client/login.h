#pragma once

#include <QObject>
#include <QWidget>
#include <QWebSocket>
#include <QFile>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent=nullptr);
    ~Login();

public slots:
    void parseResponse();

signals:
    void signedIn();
    void signUpRequired();
    void keepIn();

private slots:
    void on_registration_clicked();;
    void on_login_pressed();
    void on_login_released();
    void on_show_pressed();
    void on_password_returnPressed();
    void on_email_returnPressed();
    void on_email_textChanged(const QString &arg1);
    void on_password_textChanged(const QString &arg1);

private:
    Ui::Login *ui;
    const QString api = "http://localhost/api/v1/logIn";
    QNetworkAccessManager *m_NetAccessManager;
    QNetworkReply *m_NetReply;
    void sendPost(const QJsonDocument &request);
    void makeRequest(const QString &email,const QString &password);
};


