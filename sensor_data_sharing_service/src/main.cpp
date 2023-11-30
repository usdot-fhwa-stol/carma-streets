// Copyright 2023 Leidos
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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
    catch ( const std::exception &e) {
        SPDLOG_ERROR("Exception Encountered : {0}" , e.what());
        exit(1);
    }
    
    return 0;
}