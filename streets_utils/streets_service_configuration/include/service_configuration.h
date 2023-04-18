#pragma once

#include "streets_singleton.h"
#include "streets_configuration_exception.h"
#include "configuration.h"

#include <spdlog/spdlog.h>
#include <spdlog/async.h> //support for async logging.
#include <spdlog/sinks/daily_file_sink.h> // support for dailty file sink
#include <spdlog/sinks/stdout_color_sinks.h> // or "../stdout_sinks.h" if no colors needed
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <mutex>
#include <map>
#include <boost/filesystem.hpp>



namespace streets_service {
    /**
     * @brief Streets Configuration singleton scoped object that parses
     * manifest.json configuration file and offers static access to the
     * configuration parameter values. Singleton also configures default
     * multisink logger file and terminal logger.
     */ 
    class service_configuration  {
        private:
            /* String filepath to manifest.json configuration file */
            std::string filepath;
            /* mutex lock for streets_configuration*/
            std::mutex config_lock;
            /* Map of configuration names and values*/
            std::map< std::string, configuration > configuration_map;
            /* Time stamp for when the configuration file was last modified to only update on modifications*/
            std::time_t last_modified = 0;

            std::string _service_name;

            /**
             * @brief Constructor that takes filepath as a parameter. 
             * @param filepath relative path to manifest.json configuration file.
             */
            explicit service_configuration(const std::string &filepath = "../manifest.json");


        public:
            /**
             * @brief Parse manifest.json configuration file for CARMA-Streets service and update configurations.
             * @throws streets_configuration_exception if cannot open manifest.json configuration file or has 
             * json parse error.
             * @return rapidjson::Document containing parsed manifest.json.
             */ 
            rapidjson::Document parse_configuration_file();
            /**
             * @brief Update configuration parameters using json Document.
             * @param doc rapidjson::Document containing parsed manifest.json file.
             */ 
            void update_configuration(const rapidjson::Document &doc);
            /**
             * @brief Update loglevel for logger using json Document
             * @param doc rapidjson::Document containing parsed manifest.json file.
             */ 
            void update_log_level( const rapidjson::Document &doc );
            /**
             * @brief Helper method that reads the configurations array inside the manifest.json configuration
             * file.
             * @param arr rapidjson::GenericArray holding values from configurations element of the manifest.json file.
             * @throws streets_configuration_exception if configurations json object array is incorrectly formatted.
             */
            void parse_configurations_array( const rapidjson::GenericArray<true,rapidjson::Value> &arr);
            /**
             * @brief Method to read service level configurations from 
             * manifest.json file to configure default logger and set
             * log level.
             * @param doc rapidjson::Document containing parse manifest.json
             * configuration file. 
             */
            void configure_logger(const rapidjson::Document &doc); 
            /**
             * @brief Method to configuration spdlog default multisink logger. Includes
             * a file sink which rotates daily and terminal sink.
             * @param service_name string which is obtained form the manifest.json configuration
             * file and used as part of the naming convention for log files.
             */
            void create_default_logger( const std::string &service_name) const; 
            /**
             * @brief Method to set the spdlog::default
             * @throws streets_configuration_exception if default logger has not been configured and registered.
             */
            void set_loglevel( const std::string &loglevel) const; 

            /**
             * @brief Method to check last modified time of file to see if a file update is necessary.
             * @throws streets_configuration_exception if update_configurations fails.
             */ 
            void check_update();

            void set_service_name(const std::string &service_name);

    };
}



