#pragma once

#include <stdexcept>
namespace streets_snmp_cmd {
 /**
     * @brief Runtime error related to processing streets snmp cmd.
     */ 
    class streets_snmp_cmd_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~streets_snmp_cmd_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit streets_snmp_cmd_exception(const std::string &msg );
    };
}