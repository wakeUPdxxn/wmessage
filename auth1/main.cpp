#include <QCoreApplication>
#include "authapi.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    authApi api;
    api.launch();
    return a.exec();
}
