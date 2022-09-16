#pragma once

#include <stdexcept>



namespace traffic_signal_controller_service {
    /**
     * @brief Runtime error related to consuming desired phase plan 
     * 
     */ 
    class control_tsc_state_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~control_tsc_state_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit control_tsc_state_exception(const std::string &msg );
    };
}