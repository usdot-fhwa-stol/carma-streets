#pragma once

#include <stdexcept>



namespace streets_tsc_configuration {
    /**
     * @brief Runtime error related to processing signal_phase_and_timing JSON updates.
     * 
     */ 
    class tsc_configuration_state_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~tsc_configuration_state_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit tsc_configuration_state_exception(const std::string &msg );
    };
}