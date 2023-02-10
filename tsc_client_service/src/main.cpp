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
    try {
        if (service.initialize()){
            service.start();
        }
        else {
            SPDLOG_ERROR("TSC Service Initialization failed!");
        }
    }
    catch ( const std::runtime_error &e) {
        SPDLOG_ERROR("Exception Encountered : {0}" , e.what());
        exit(1);
    }
    
    return QCoreApplication::exec();
}