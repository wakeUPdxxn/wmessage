#pragma once

#include <QMainWindow>
#include <QMessageBox>
#include <QWebSocket>
#include <QDialog>
#include <QLabel>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWidget>
#include <QListWidgetItem>
#include <QFormLayout>
#include <QtConcurrent>

QT_BEGIN_NAMESPACE
namespace Ui { class Client;}
QT_END_NAMESPACE

class Client : public QMainWindow
{
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);
    ~Client();

public slots:
    void messageReceived(const QString &message);
    void responseReceived(const QByteArray &response);
    void showSignUpForm();
    void showSignInForm();
    void swithAuthorizedState();
    void sendCallToServer();

private slots:
    //void on_action_triggered();
    void on_me_pressed();
    void on_me_released();
    void on_search_textChanged(const QString &arg1);
    void on_search_editingFinished();
    void on_dialogsList_itemDoubleClicked(QListWidgetItem *item);
    void on_message_returnPressed();

private:
    Ui::Client *ui;
    QByteArray Data;
    QWebSocket *socket;
    bool chatSelected=false;
    bool isAuthorized = false;
    QString chatSearchData;
    QString selfNickName;
    QString selfUID;
    struct{
        QString nickName;
        QString UID;
        QString status;
    }currentChat;
    QList<QVariantMap>usersInSearch;
    void parseUserData();
};

