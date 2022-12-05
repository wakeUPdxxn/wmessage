#ifndef LOGIN_H
#define LOGIN_H

#include <QTcpSocket>
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
    void on_registration_clicked();
    void on_login_clicked();

    void on_login_pressed();

    void on_login_released();

    void on_email_editingFinished();

    void on_email_selectionChanged();

    void on_pushButton_clicked();

    void on_show_clicked();

    void on_show_released();

    void on_show_pressed();

    void on_email_textEdited(const QString &arg1);

    void on_password_textEdited(const QString &arg1);

    void on_password_editingFinished();

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
