#ifndef LOGIN_H
#define LOGIN_H

#include "registration.h"

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr,QTcpSocket *sock=nullptr);
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
    QTcpSocket *socket;
    Registration *reg;
    struct RESPONSE{
      QString payload;
      QString email;
      QString password;
    }response;
};

#endif // LOGIN_H
