#pragma once

#include <string>

namespace streets_service {

    static const std::string LOGS_DIRECTORY_ENV = "LOGS_DIRECTORY";

    static const std::string DEFAULT_LOGS_DIRECTORY = "../logs/";

    static const std::string SIMULATION_MODE_ENV = "SIMULATION_MODE";

    static const std::string DEFAULT_SIMULATION_MODE = "FALSE";

    static const std::string CONFIG_FILE_PATH_ENV = "CONFIG_FILE_PATH";
    
    static const std::string DEFAULT_CONFIG_FILE_PATH = "../manifest.json";

    static const std::string TIME_SYNC_TOPIC_ENV = "TIME_SYNC_TOPIC";

    static const std::string DEFAULT_TIME_SYNC_TOPIC = "time_sync";

    
}