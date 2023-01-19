#pragma once

#include <QMainWindow>
#include <QMessageBox>
#include <QWebSocket>
#include <QDialog>
#include <QLabel>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui { class Client;}
QT_END_NAMESPACE

class Client : public QMainWindow
{
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);
    ~Client();
    bool isAuthorized();

public slots:
    void messageReceived(const QString &message);
    void binaryMessageReceived(const QByteArray &msg);
    void showSignUpForm();
    void showSignInForm();
    void swithAuthorizedState();

private slots:
    void on_action_triggered();
    void on_lineEdit_returnPressed();

private:
    Ui::Client *ui;
    QByteArray Data;
    QWebSocket *socket;
    void sendMessage(const QString &message);
    
};

