
#include "OAIDefaultApi.h"
#include <QTimer>
#include "intersection_client.h"
#include <QEventLoop>
#include <QCoreApplication>
#include "OAIHelpers.h"

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);
    intersection_client client;
    client.call();
    return 0;
}