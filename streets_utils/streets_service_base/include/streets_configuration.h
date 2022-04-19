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
#include <boost/filesystem/operations.hpp>



namespace streets_service {
    /**
     * @brief Streets Configuration singleton scoped object that parses
     * manifest.json configuration file and offers static access to the
     * configuration parameter values. Singleton also configures default
     * multisink logger file and terminal logger.
     */ 
    class streets_configuration : public streets_singleton<streets_configuration> {
        friend class streets_singleton<streets_configuration>;

        private:
            /* String filepath to manifest.json configuration file */
            std::string filepath;
            /* mutex lock for streets_configuration*/
            std::mutex config_lock;
            /* Map of configuration names and values*/
            std::map< std::string, configuration > configuration_map;
            /* Time stamp for when the configuration file was last modified to only update on modifications*/
            std::time_t last_modified;

            /**
             * @brief Constructor that takes filepath as a parameter. 
             * @param filepath relative path to manifest.json configuration file.
             */
            explicit streets_configuration(const std::string &filepath = "../manifest.json");


        public:
            /**
             * @brief Retreive configuration parameter string value with provided name.
             * @param config_param_name configuration parameter name.
             * @throw streets_configuration_exception if configuration is not of data_type STRING.
             * @return string configuration value. 
             */ 
            static std::string get_string_config( const std::string &config_param_name);
            /**
             * @brief Retreive configuration parameter integer value with provided name.
             * @param config_param_name configuration parameter name.
             * @throws streets_configuration_exception if configuration is not of data_type INTEGER.
             * @return int configuration value. 
             */ 
            static int get_int_config( const std::string &config_param_name);
            /**
             * @brief Retreive configuration parameter double value with provide name.
             * @param config_param_name configuration parameter name.
             * @throws streets_configuration_exception if configuration is not of data_type DOUBLE.
             * @return double configuration value.
             */ 
            static double get_double_config( const std::string &config_param_name);
            /**
             * @brief Retreive configuration parameter boolean value with provide name.
             * @param config_param_name configuration parameter name.
             * @throws streets_configuration_exception if configuration is not of data_type BOOL.
             * @return bool configuration value.
             */ 
            static bool get_boolean_config( const std::string &config_param_name);
            /**
             * @brief Static method to initialize spdlog default logger. 
             */ 
            static void initialize_logger();
        
        protected:

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
            void configure_logger(const rapidjson::Document &doc) const; 
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

            // Hide get_singleton method. Use static methods instead.
            using streets_singleton::get_singleton;

    };
}



