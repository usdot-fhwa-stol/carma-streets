#include <iostream>
#include <thread>
#include <spdlog/spdlog.h>
#include "streets_configuration.h"
#include "sensor_data_sharing_service.hpp"

int main(int argc, char **argv)
{
       
    sensor_data_sharing_service::sds_service service;
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
    
    return 0;
}