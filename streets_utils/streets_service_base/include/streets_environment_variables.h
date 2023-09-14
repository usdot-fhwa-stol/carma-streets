#pragma once

#include <string>

namespace streets_service {

    inline static const std::string LOGS_DIRECTORY_ENV = "LOGS_DIRECTORY";

    inline static const std::string DEFAULT_LOGS_DIRECTORY = "../logs/";

    inline static const std::string SIMULATION_MODE_ENV = "SIMULATION_MODE";

    inline static const std::string DEFAULT_SIMULATION_MODE = "FALSE";

    inline static const std::string CONFIG_FILE_PATH_ENV = "CONFIG_FILE_PATH";
    
    inline static const std::string DEFAULT_CONFIG_FILE_PATH = "../manifest.json";

    inline static const std::string TIME_SYNC_TOPIC_ENV = "TIME_SYNC_TOPIC";

    inline static const std::string DEFAULT_TIME_SYNC_TOPIC = "time_sync";

    
}