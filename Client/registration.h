#pragma once

#include <QObject>
#include <QWidget>
#include <QWebSocket>

namespace Ui {
class Registration;
}

class Registration : public QWidget
{
    Q_OBJECT

public:
    explicit Registration(QWidget *parent,QWebSocket *sock);
    ~Registration();

public slots:

    void responseReceived(const QByteArray &response);

signals:
    void signedUp();

private slots:

    void on_registrate_pressed();
    void on_registrate_released();
    void on_show_pressed();
    void on_email_returnPressed();
    void on_password_returnPressed();
    void on_username_returnPressed();

private:
    Ui::Registration *ui;
    QWebSocket *socket;
    QByteArray Data;
    struct Request{
      QString payload;
      QString nickname;
      QString email;
      QString password;
    }request;
    void sendData();
};
