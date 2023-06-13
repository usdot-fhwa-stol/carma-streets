#pragma once

#include <stdexcept>



namespace streets_phase_control_schedule {
    /**
     * @brief Runtime error related to processing desired phase plan.
     */ 
    class streets_phase_control_schedule_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~streets_phase_control_schedule_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit streets_phase_control_schedule_exception(const std::string &msg );
    };
}