#pragma once

#include <stdexcept>



namespace traffic_signal_controller_service {
    /**
     * @brief Runtime error related to SNMP client used to communicate with Traffic Signal Controller (NTCIP).
     * 
     * @author Paul Bourelly 
     */ 
    class snmp_client_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~snmp_client_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit snmp_client_exception(const std::string &msg );
    };
}