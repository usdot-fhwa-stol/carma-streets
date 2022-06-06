#include <QTimer>
#include "intersection_client.h"
#include <QEventLoop>
#include <QCoreApplication>
#include "intersection_server_api_lib/OAIHelpers.h"

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);
    intersection_client client;
    client.call();
    return 0;
}