#pragma once

#include <stdexcept>



namespace traffic_signal_controller_service {
    /**
     * @brief Runtime error related to SNMP client used to communicate with Traffic Signal Controller (NTCIP).
     * 
     */ 
    class monitor_states_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~monitor_states_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit monitor_states_exception(const std::string &msg );
    };
}