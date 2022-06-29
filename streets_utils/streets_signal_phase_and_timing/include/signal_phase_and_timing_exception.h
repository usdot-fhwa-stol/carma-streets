#pragma once

#include <stdexcept>



namespace signal_phase_and_timing {
    /**
     * @brief Runtime error related to processing signal_phase_and_timing JSON updates.
     * 
     * @author Paul Bourelly 
     */ 
    class signal_phase_and_timing_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~signal_phase_and_timing_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit signal_phase_and_timing_exception(const std::string &msg );
    };
}