#pragma once
#include <QObject>
#include <QtHttpServer>
#include <QtConcurrent>
#include "exception.h"
#include "requesthandler.h"

class authApi:public QObject
{
    Q_OBJECT
public:
    explicit authApi(QObject *parent = nullptr);
    void launch();
private:
    QHttpServer *p_m_httpServer;
    Exception *p_m_Exception;
    RequestHandler *p_m_RequestHandler;
    mutable QHostAddress listeningAddr = QHostAddress::SpecialAddress::LocalHost;
    mutable qint16 listeningPort = 80;
    void makeRouts();
};

