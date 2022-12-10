#pragma once

#include <QWebSocket>
#include <QObject>
#include <QWidget>

namespace Ui {
class Registration;
}

class Registration : public QWidget
{
    Q_OBJECT

public:
    explicit Registration(QWidget *parent,QWebSocket *sock);
    ~Registration();
    void SendLoginPassword();

private slots:
    void on_registrate_pressed();

    void on_registrate_released();

    void on_show_pressed();

    void on_email_returnPressed();

    void on_password_returnPressed();

private:
    Ui::Registration *ui;
    QWebSocket *socket;
    QByteArray Data;
    struct RESPONSE{
      QString payload;
      QString email;
      QString password;
    }response;
};
