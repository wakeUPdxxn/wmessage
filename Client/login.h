#pragma once

#include "registration.h"
#include <QWebSocket>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    Login(QWidget *parent,QWebSocket *sock);
    ~Login();

public slots:
    void responseReceived(const QByteArray &response);

signals:
    void signedIn();
    void signUpRequired();

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
    QWebSocket *socket;
    Registration *reg;
    struct Request{
      QString payload;
      QString email;
      QString password;
    }request;
    void sendLoginPassword();
};


