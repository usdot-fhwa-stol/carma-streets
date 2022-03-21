#pragma once

#include <fstream>

/**
 * @brief Streets Configuration Exception thrown when manifest.json
 * configuration files are incorrectly formatted or Streets configuration
 * singleton is misused. 
 */ 
class streets_configuration_exception : public std::runtime_error{
    public:
        /**
         * @brief Destructor.
         */ 
        ~streets_configuration_exception();
        /**
         * @brief Constructor. 
         * @param msg String exception message.
         */  
        streets_configuration_exception(const std::string msg );
};

