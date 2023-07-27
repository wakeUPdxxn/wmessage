#include <QCoreApplication>
#include <server.h>
#include <authentication.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server server;
    //Authentication auth(nullptr);
    //auth.setApiKey(QString("AIzaSyArYs6ByVk8DB9QU7eQGenWk_QOZBMLPCw"));
    //auth.signUserUp(QString("pexwtrt@yandex.ru"),QString("psase4343"),QString("dxxn"));
    return a.exec();
}
