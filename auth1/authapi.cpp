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
    openTheRouts();
}

void authApi::openTheRouts()
{
    p_m_httpServer->route("/api/v1/logIn",QHttpServerRequest::Method::Post,[this](const QHttpServerRequest &request){
        return QtConcurrent::run([this, &request] () {

            p_m_RequestHandler = new RequestHandler();

            if(p_m_RequestHandler->checkRequest(request)){

                QString userRefreshToken="token";
                QHttpServerResponse response(userRefreshToken,QHttpServerResponse::StatusCode::Ok);
                response.setHeader("Access-Control-Allow-Origin","*");

                delete p_m_RequestHandler;
                return response;
            }
            else {
                QHttpServerResponse response(QHttpServerResponse::StatusCode::BadRequest);
                response.setHeader("Access-Control-Allow-Origin","*");
                return response;
            }

        });
    });

    //more routs;
}
