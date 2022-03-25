#pragma once

#include "streets_configuration_exception.h"

#include <rapidjson/document.h>
#include <spdlog/spdlog.h>


namespace streets_service {
    /**
     * @enum Data type enum for configurations.
     */ 
    enum class data_type { config_string=0, config_int=1, config_double=2, config_bool=3 }; 

    /**
     * @struct Configuration struct to store configuration parameter name, value, 
     * description and data type. Also contains methods to deserialize from json
     * and operators for equivalency.
     */
    struct configuration {
        /**
         * Name of configuration
         */
        std::string name;
        /**
         * Value of configuration
         */ 
        std::string value;
        /**
         * Description of configuration
         */ 
        std::string description;
        /**
         * Data type enum for configuration
         */
        data_type type;

        /**
         * @brief operator== checks if two instances are equal.
         * @param t configuration comparison.
         */ 
        bool operator==(const configuration& t) const;

        /**
         * @brief operator!= checks if two instances are not equal.
         * @param t configuration comparison.
         */ 
        bool operator!=(const configuration& t) const;
        /**
         * @brief Method to translate json object to configuration struct.
         * @param val rapidjson::Value object representing configuration.
         */ 
        void fromJson(const rapidjson::Value &val );
    };
}