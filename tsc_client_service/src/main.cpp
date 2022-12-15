#include <iostream>
#include <thread>
#include <QCoreApplication>
#include <spdlog/spdlog.h>
#include "streets_configuration.h"
#include "tsc_service.h"

int main(int argc, char **argv)
{
       
    QCoreApplication a(argc, argv);
    std::string setCustomMibsCommand = "export MIBS=ALL";
    system(setCustomMibsCommand.c_str()); 

    streets_service::streets_configuration::initialize_logger();
    traffic_signal_controller_service::tsc_service service;
    if (service.initialize()){
        service.start();
    }
    else {
        SPDLOG_ERROR("TSC Service Initialization failed!");
    }
    
    return QCoreApplication::exec();
}