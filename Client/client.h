#pragma once

#include "login.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QWebSocket>
#include <QDialog>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class Client;}
QT_END_NAMESPACE

class Client : public QMainWindow
{
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);
    ~Client();

private slots:
    void on_action_triggered();
    void on_lineEdit_returnPressed();

public slots:
    void readyRead(const QString &message);

private:
    Ui::Client *ui;
    QByteArray Data;
    QWebSocket *socket;
    Login *log;
    bool authorized=0;
    void login();
    void SendToServer(QString str);
    
};

