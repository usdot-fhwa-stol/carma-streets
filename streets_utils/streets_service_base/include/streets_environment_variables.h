#pragma once

#include <string>

namespace streets_service {

    inline const std::string LOGS_DIRECTORY_ENV = "LOGS_DIRECTORY";

    inline const std::string DEFAULT_LOGS_DIRECTORY = "../logs/";

    inline const std::string SIMULATION_MODE_ENV = "SIMULATION_MODE";

    inline const std::string DEFAULT_SIMULATION_MODE = "FALSE";

    inline const std::string CONFIG_FILE_PATH_ENV = "CONFIG_FILE_PATH";
    
    inline const std::string DEFAULT_CONFIG_FILE_PATH = "../manifest.json";

    inline const std::string TIME_SYNC_TOPIC_ENV = "TIME_SYNC_TOPIC";

    inline const std::string DEFAULT_TIME_SYNC_TOPIC = "time_sync";

    
}