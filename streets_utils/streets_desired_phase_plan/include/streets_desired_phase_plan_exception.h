#pragma once

#include <stdexcept>



namespace streets_desired_phase_plan {
    /**
     * @brief Runtime error related to processing desired phase plan.
     */ 
    class streets_desired_phase_plan_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~streets_desired_phase_plan_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit streets_desired_phase_plan_exception(const std::string &msg );
    };
}