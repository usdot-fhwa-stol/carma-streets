#include <QCoreApplication>
#include "signal_opt_service.h"

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);
    signal_opt_service::signal_opt_service so_service;
    if (so_service.initialize())
    {
        so_service.start();
    }
    return 0;
}
