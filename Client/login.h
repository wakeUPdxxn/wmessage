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
    explicit Login(QWidget *parent = nullptr,QWebSocket *sock=nullptr);
    ~Login();
    void SendLoginPassword();

private slots:    
    void on_registration_clicked();;

    void on_login_pressed();

    void on_login_released();

    void on_show_pressed();

    void on_password_returnPressed();

    void on_email_returnPressed();

private:
    Ui::Login *ui;
    QByteArray Data;
    QWebSocket *socket;
    Registration *reg;
    struct RESPONSE{
      QString payload;
      QString email;
      QString password;
    }response;
};


