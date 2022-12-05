#ifndef CLIENT_H
#define CLIENT_H

#include "login.h"
#include <QMainWindow>
#include <QMessageBox>
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
    void ReadyRead();

private:
    Ui::Client *ui;
    QByteArray Data;
    QTcpSocket *socket;
    quint16 nextBlockSize;
    Login *log;
    bool authorized=0;
    void login();
    void SendToServer(QString str);
    
};
#endif // CLIENT_H
