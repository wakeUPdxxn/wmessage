#include <QCoreApplication>
#include "authapi.h"
#include "exception.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    authApi api;
    try {
        api.launch();
    }
    catch(const Exception &exception){
        qDebug () << exception.what();
    }
    return a.exec();
}
