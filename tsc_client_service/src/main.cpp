#include <iostream>
#include <thread>
#include <QCoreApplication>
#include "streets_configuration.h"
#include "tsc_service.h"

int main(int argc, char **argv)
{
       
    QCoreApplication a(argc, argv);
    std::string setCustomMibsCommand = "export MIBS=ALL";
    system(setCustomMibsCommand.c_str()); 

    streets_service::streets_configuration::initialize_logger();
    traffic_signal_controller_service::tsc_service service;
    service.initialize();
    service.start();
    return a.exec();
}