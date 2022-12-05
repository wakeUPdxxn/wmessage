#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QTcpSocket>
#include <QObject>
#include <QWidget>

namespace Ui {
class Registration;
}

class Registration : public QWidget
{
    Q_OBJECT

public:
    explicit Registration(QWidget *parent,QTcpSocket *sock);
    ~Registration();
    void SendLoginPassword();

private slots:
    void on_registrate_pressed();

    void on_registrate_released();

    void on_show_pressed();

private:
    Ui::Registration *ui;
    QTcpSocket *socket;
    QByteArray Data;
    struct RESPONSE{
      QString payload;
      QString email;
      QString password;
    }response;
};

#endif // REGISTRATION_H
