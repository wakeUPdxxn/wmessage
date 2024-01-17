#include "authapi.h"

authApi::authApi(QObject *parent)
{
    p_m_httpServer = new QHttpServer(parent);
}

void authApi::launch()
{
    if(!p_m_httpServer->listen(listeningAddr,listeningPort)){
        throw p_m_Exception=new Exception("An error occured during the listening");
    }
    else{
        qDebug() << "Api is working";
    }
    makeRouts();
}

void authApi::makeRouts()
{
    p_m_httpServer->route("/api/v1/logIn",QHttpServerRequest::Method::Post,[this](const QHttpServerRequest &request){
        return QtConcurrent::run([this, &request] () {

            p_m_RequestHandler = new RequestHandler();

            QVariantMap parseResult=p_m_RequestHandler->checkRequest(request,QString("logIn"));
            if (parseResult.contains("error")) {
                QHttpServerResponse response(QJsonDocument::fromVariant(parseResult).object(),QHttpServerResponse::StatusCode::BadRequest);
                response.setHeader("Access-Control-Allow-Origin","*");
                return response;
            }
            else {
                QVariantMap userData=parseResult;

                QString email=userData["email"].toString();
                QString password=userData["password"].toString();
                QString returnSecureToken=userData["returnSecureToken"].toString();
                //db call
                QString refreshToken="token";
                QJsonObject responseData({QPair<QString,QJsonValue>("refreshToken","value")});
                QHttpServerResponse response(responseData,QHttpServerResponse::StatusCode::Ok);
                response.setHeader("Access-Control-Allow-Origin","*");

                delete p_m_RequestHandler;
                return response;
            }
        });
    });
    p_m_httpServer->route("/api/v1/signIn",QHttpServerRequest::Method::Post,[this](const QHttpServerRequest &request){
        return QtConcurrent::run([this, &request] () {

            p_m_RequestHandler = new RequestHandler();

            QVariantMap parseResult=p_m_RequestHandler->checkRequest(request,QString("signIn"));
            if (parseResult.contains("error")) {
                if(parseResult["reason"]=="undefiened"){
                    //check fills on db;
                }
                QHttpServerResponse response(QJsonDocument::fromVariant(parseResult).object(),QHttpServerResponse::StatusCode::BadRequest);
                response.setHeader("Access-Control-Allow-Origin","*");
                return response;
            }
            else {
                QVariantMap userData=parseResult;

                QString email=userData["email"].toString();
                QString password=userData["password"].toString();
                QString returnSecureToken=userData["returnSecureToken"].toString();
                //db call
                QString refreshToken="token";
                QJsonObject responseData({QPair<QString,QJsonValue>("refreshToken","value")});
                QHttpServerResponse response(responseData,QHttpServerResponse::StatusCode::Ok);
                response.setHeader("Access-Control-Allow-Origin","*");

                delete p_m_RequestHandler;
                return response;
            }
        });
    });
}
